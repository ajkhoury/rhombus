// Copyright 2009 Nick Johnson

#include <lib.h>
#include <trap.h>
#include <task.h>
#include <mem.h>

// Handles IRQ 0, and advances a simple counter used as a clock
image_t *pit_handler(image_t *state) {
	static u32int tick = 0;
	if (state->cs & 0x3) tick++;

	return task_switch(next_task(0));
}

image_t *fault_generic(image_t *image) {
	if ((image->cs & 0x3) == 0) {
		printk("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		panic("unknown exception");
	}
	return signal(curr_pid, S_GEN, image->num | (image->err << 16), 0, 0, 0, TF_NOERR);
}

image_t *fault_page(image_t *image) {
	u32int cr2; asm volatile ("movl %%cr2, %0" : "=r" (cr2));
	if ((image->cs & 0x3) == 0) { // i.e. if it was kernelmode
		printk("page fault at %x, frame %x, ip = %x\n", cr2, page_get(cr2), image->eip);
		panic("page fault exception");
	}
	return signal(curr_pid, S_PAG, 0, page_get(cr2), 0, cr2, TF_NOERR);
}

image_t *fault_float(image_t *image) {
	if ((image->cs & 0x3) == 0) panic("floating point exception");
	return signal(curr_pid, S_FPE, image->eip, 0, 0, 0, TF_NOERR);
}

image_t *fault_double(image_t *image) {
	printk("DS:%x CS:%x\n", image->ds, image->cs);
	panic("double fault exception");
	return NULL;
}

// Note - different than UNIX / POSIX fork() -
// parent gets child PID, child gets *negative* parent PID, 0 is error
// I think it's much more useful, but the libc can convert it easily

image_t *fork_call(image_t *image) {
	u16int parent = curr_pid;
	task_t *child = new_task(get_task(curr_pid));
	if (child->magic != 0x4224) ret(image, 0);
	image->eax = child->pid;
	image = task_switch(child);
	ret(image, -parent);
}

image_t *exit_call(image_t *image) {
	u16int dead_task = curr_pid;
	u32int ret_val = image->eax;
	if (dead_task == 1) {
		asm volatile ("sti");
		asm volatile ("hlt");
	}
	task_t *t = get_task(dead_task);
	map_clean(t->map);
	map_free(t->map);
	rem_task(get_task(dead_task));
	return signal(t->parent, S_DTH, ret_val, 0, 0, 0, TF_NOERR);
}

image_t *sint_call(image_t *image) {
	return signal(image->edi, image->esi & 0xFF, 
		image->eax, image->ebx, image->ecx, image->edx, (image->esi >> 8) & 0xFF);
}

image_t *sret_call(image_t *image) {
	return sret(image);
}

image_t *eout_call(image_t *image) {
	printk("%s", image->eax);
	return image;
}

image_t *irq_redirect(image_t *image) {
	return signal(irq_holder[DEIRQ(image->num)], S_IRQ, DEIRQ(image->num), 0, 0, 0, TF_NOERR);
}

image_t *rirq_call(image_t *image) {
	task_t *t = get_task(curr_pid);
	if (t->user.ring > 1) ret(image, EPERMIT);
	irq_holder[image->eax % 15] = curr_pid;
	register_int(IRQ(image->eax), irq_redirect);
	ret(image, 0);
}

image_t *mmap_call(image_t *image) {
	u32int dst = image->edi;

	// Bounds check page address
	if (dst + image->ecx > LSPACE) ret(image, EPERMIT);

	// Allocate pages with flags
	for (dst &= ~0xFFF; dst < (image->edi + image->ecx); dst += 0x1000) {
		if (!(page_get(dst) & 0x1)) {
			u32int page = page_fmt(frame_new(), (image->ebx & PF_MASK) | PF_PRES | PF_USER); 
			page_set(dst, page);
		}
	}

	ret(image, 0);
}

image_t *umap_call(image_t *image) {
	u32int dst = image->edi;

	// Bounds check page address
	if (dst + image->ecx > LSPACE) ret(image, EPERMIT);

	// Free pages
	for (dst &= ~0xFFF; dst < (image->edi + image->ecx); dst += 0x1000) {
		if (page_get(dst) & PF_PRES) {
			if (page_get(dst) & PF_LINK) page_set(dst, 0);
			else p_free(dst);
		}
	}

	ret(image, 0);
}

image_t *rmap_call(image_t *image) {
	u32int src = image->esi;
	u32int dst = image->edi;
	u32int flags = image->ebx;

	// Bounds check both addresses
	if (src + image->ecx > LSPACE || dst + image->ecx > LSPACE) ret(image, EPERMIT);
	if (src & 0xFFF || dst & 0xFFF) ret(image, EPERMIT);

	// Move pages
	for (; dst<image->edi+image->ecx && src<image->esi+image->ecx; dst += 0x1000, src += 0x1000) {
		if ((page_get(src) & 0x1) && ((page_get(dst) & 0x1) == 0)) {
			page_set(dst, page_fmt(page_get(src), flags));
			page_set(src, 0);
		}
	}
	
	ret(image, 0);
}

image_t *fmap_call(image_t *image) {
	u32int src = image->esi;
	u32int dst = image->edi;
	u32int flags = image->ebx;
	task_t *t = get_task(curr_pid);
	task_t *src_t;

	// Bounds check destination
	if (dst > LSPACE) ret(image, EPERMIT);

	// Set physical address if chosen (eax == 0)
	if (image->eax == 0) {
		page_set(dst, src | (flags & PF_MASK));
		ret(image, 0);
	}

	// Bounds check source
	if (src > LSPACE || t->user.ring > 0) ret(image, EPERMIT);

	// Map source map
	src_t = get_task(image->eax);
	map_temp(src_t->map);

	// Check source
	if ((tmap[src >> 22] & 0x1) == 0 || ttbl[src >> 12] & 0x1 == 0) ret(image, EREPEAT);

	// Check destination
	if (cmap[dst >> 22] & 0x1 || ctbl[src >> 12] & 0x1) ret(image, EREPEAT);

	// Move page
	page_set(dst, ttbl[src >> 12] | PF_LINK | PF_WRTT | PF_DISC);
	ttbl[src >> 12] |= PF_WRTT | PF_DISC;

	page_flush();

	ret(image, 0);
}
