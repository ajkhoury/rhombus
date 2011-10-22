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

#include <rhombus.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <rho/natio.h>

/*****************************************************************************
 * fs_find
 *
 * Finds the robject with the given path <path> if it exists and returns a
 * robject pointer to it. If it does not exist, this function returns RP_NULL.
 */

rp_t fs_find(const char *path) {
	uint64_t rp;
	uint64_t root;
	char *reply;
	char *path_s;

	// if path is NULL, return NULL
	if (!path) {
		return RP_NULL;
	}

	// if preceeded by a resource pointer, use that as root and strip it
	if (path[0] == '@') {
		root = ator(path);
		while (*path != '/' && *path) path++;
	}
	else {
		root = fs_root;
	}

	// simply return root if path is nonexistent
	if (path[0] == '\0' || (path[0] == '/' && path[1] == '\0')) {
		return root;
	}

	path_s = path_simplify(path);
	if (!path_s) return RP_NULL;

	reply = rcall(root, "find %s", path_s);
	free(path_s);

	if (!reply) {
		errno = ENOSYS;
		return RP_NULL;
	}

	if (reply[0] == '!') {
		if      (!strcmp(reply, "! nfound")) errno = ENOENT;
		else if (!strcmp(reply, "! denied")) errno = EACCES;
		else if (!strcmp(reply, "! nosys"))  errno = ENOSYS;
		else if (!strcmp(reply, "! notdir")) errno = ENOTDIR;
		else                                 errno = EUNK;
		free(reply);
		return RP_NULL;
	}

	if (reply[0] == '>' && reply[1] == '>' && reply[2] == ' ') {
		rp = fs_find(&reply[3]);
		free(reply);
		return rp;
	}	

	rp = ator(reply);
	free(reply);

	return rp;
}
