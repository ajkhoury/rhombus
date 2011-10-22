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

#include <stdio.h>

#include <rho/natio.h>

int rename(const char *oldpath, const char *newpath) {
//	uint64_t old, newdir, new;

	// this function is stubbed until hardlinks are working
	return 1;
}

//	/* attempt to find old file */
//	old = fs_find(0, oldpath);
//
//	if (!old) {
//		return 1;
//	}
//
//	/* attempt to find new directory */
//	newdir = fs_find(0, path_parent(newpath));
//
//	if (!newdir) {
//		return 1;
//	}
//
//	/* attempt to move the file */
//	new = fs_move(newdir, path_name(newpath), old);
//
//	if (new) {
//		return 0;
//	}
//	else {
//		return 1;
//	}
//}
