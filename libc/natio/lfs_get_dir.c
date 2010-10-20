/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <string.h>
#include <stdlib.h>
#include <natio.h>

/****************************************************************************
 * lfs_get_dir
 *
 * Returns the directory containing the LFS node at <path> from <root>.
 * Returns null on error.
 */

struct lfs_node *lfs_get_dir(struct lfs_node *root, const char *path) {
	struct lfs_node *node;
	char *dir_path, *tail;

	dir_path = strdup(path);
	tail = strrchr(dir_path, PATH_SEP);

	if (tail) {
		*tail = '\0';
	}
	else {
		free(dir_path);
		dir_path = strdup("");
	}

	node = lfs_get_path(root, dir_path);

	free(dir_path);

	if (node && node->type == VFS_DIR) {
		return node;
	}
	else {
		return NULL;
	}
}
