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

#include <rho/mutex.h>

/****************************************************************************
 * fseek
 *
 * Moves the file position of a stream to <offset> bytes from the position
 * specified by <whence>.
 */

int fseek(FILE *stream, fpos_t offset, int whence) {

	fflush(stream);

	mutex_spin(&stream->mutex);

	switch (whence) {
	case SEEK_CUR:
		stream->position += offset;
		break;
	case SEEK_END:
		stream->position = stream->size - offset;
		break;
	case SEEK_SET:
		stream->position = offset;
		break;
	}

	mutex_free(&stream->mutex);

	return 0;
}
