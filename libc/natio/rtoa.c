/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <stdio.h>
#include <proc.h>

/*****************************************************************************
 * rtoa
 *
 * Converts the resource pointer <rp> to canonical string format.
 */

char *rtoa(uint64_t rp) {

	if (!rp) {
		return strdup("@nil");
	}
	else {
		return saprintf("@%d:%d", RP_PID(rp), RP_INDEX(rp));
	}
}

char *rtoa_static(uint64_t rp) {
	char *str;
	static char *str_static[MAX_THREADS];

	str = rtoa(rp);

	if (!str_static[gettid()]) {
		str_static[gettid()] = malloc(sizeof(char) * 42);
	}

	strcpy(str_static[gettid()], str);
	free(str);

	return str_static[gettid()];
}
