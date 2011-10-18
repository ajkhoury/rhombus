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
#include <natio.h>
#include <errno.h>
#include <mutex.h>

/****************************************************************************
 * fwrite
 *
 * Writes <nmemb> * <size> bytes of data from <stream> from the buffer <ptr>.
 * Returns the number of bytes / <size> successfully written; on error or 
 * EOF, a lessened or zero value is returned.
 */

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	const uint8_t *data = ptr;
	size_t i, ret;
	
	if (!stream) {
		return 0;
	}

	if (!(size * nmemb)) {
		return 0;
	}

	mutex_spin(&stream->mutex);

	if (stream->flags & FILE_NBF) {

		ret = rp_write(fd_rp(stream->fd), (void*) ptr, size * nmemb, stream->position);
		stream->position += ret;

		if (size == 0) {
			size = 1;
		}

		mutex_free(&stream->mutex);

		return (ret / size);
	}

	for (i = 0; i < size * nmemb; i++) {
		stream->buffer[stream->buffpos++] = data[i];

		if (stream->flags & FILE_LBF) {
			if ((data[i] == '\n') || (stream->buffpos > stream->buffsize)) {
				mutex_free(&stream->mutex);
				fflush(stream);
			}
		}
		else {
			if (stream->buffpos >= stream->buffsize) {
				mutex_free(&stream->mutex);
				fflush(stream);
			}
		}
	}

	return nmemb;
}
