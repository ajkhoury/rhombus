/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <thread.h>
#include <debug.h>
#include <space.h>
#include <cpu.h>

/*****************************************************************************
 * fault_generic
 *
 * General fault handler for anything that cannot be handled usefully. If the
 * fault is in userspace, it freezes the current process and sends it a
 * PORT_ILL message. If the fault is in kernelspace, it panics.
 */

struct thread *fault_generic(struct thread *image) {

	/* If in kernelspace, panic */
	if ((image->cs & 0x3) == 0) {
		debug_printf("EIP:%x NUM:%d ERR:%x\n", image->eip, image->num, image->err);
		debug_panic("unknown exception");
	}

	/* fault */
	debug_printf("%d: generic exception %d, ip = %x, errcode %x\n", 
		image->proc->pid, image->num, image->err, image->eip);
	debug_printf("user stack: %x - %x\n", image->stack, image->stack + SEGSZ);
	debug_printf("user stack dump: (ebp = %x)\n", image->ebp);
	debug_dumpi((void*) image->useresp, 30);

	process_freeze(image->proc);
	return thread_send(image, image->proc->pid, PORT_ILL, NULL);
}
