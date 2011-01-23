/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <interrupt.h>
#include <process.h>

/*****************************************************************************
 * syscall_user (int 0x4d)
 *
 * ECX: pid
 *
 * Returns the user id of the process with pid <pid> on success, -1 on
 * failure. If <pid> is 0, returns the effective user id of the current
 * thread.
 *
 * Note: this is a good way of determining the existence of a process.
 */

struct thread *syscall_user(struct thread *image) {
	struct process *proc;
	
	if (image->ecx) {
		proc = process_get(image->ecx);
	
		if (proc) {
			image->eax = proc->user;
		}
		else {
			image->eax = -1;
		}
	}
	else {
		image->eax = image->user;
	}

	return image;
}
