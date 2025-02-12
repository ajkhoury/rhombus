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

#include "fbterm.h"

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <rho/mutex.h>
#include <rho/proc.h>

struct line {
	struct line *next;
	struct line *prev;
	char *data;
};

static struct line *line_buf_in;
static struct line *line_buf_out;
static char *_line;
static size_t line_pos;
static char *buffer;
static size_t buffer_top;
static size_t buffer_size;
static bool mutex;
static uint32_t waiting_tid;

void _push_line(char *l) {
	struct line *line;
	uint32_t tid;
	
	line = calloc(sizeof(struct line), 1);
	line->data = l;

	mutex_spin(&mutex);
	if (!line_buf_in) {
		line_buf_in  = line;
		line_buf_out = line;
		line->next = NULL;
		line->prev = NULL;
	}
	else {
		line->next = NULL;
		line->prev = line_buf_out;
		line_buf_out->next = line;
		line_buf_out = line;
	}

	tid = waiting_tid;
	waiting_tid = 0;
	mutex_free(&mutex);

	if (tid) {
		wake(tid - 1);
	}
}

char *_pop_line(void) {
	struct line *line;
	char *data;

	mutex_spin(&mutex);
	if (!line_buf_out) {
		while (1) {
			waiting_tid = gettid() + 1;
			mutex_free(&mutex);
			stop();
			mutex_spin(&mutex);
			if (line_buf_out) break;
		}
	}

	line = line_buf_out;
	line_buf_out = line_buf_out->prev;
	if (!line_buf_out) line_buf_in = NULL;
	mutex_free(&mutex);

	data = line->data;
	free(line);

	return data;
}

int fbterm_buffer(int c) {

	if (!buffer) {
		buffer = malloc(256);
		
		if (!buffer) {
			abort();
		}

		buffer_size = 256;
		buffer_top = 0;
	}

	if (c == '\b') {
		if (buffer_top) buffer_top--;
	}
	else {
		buffer[buffer_top++] = c;
		if (buffer_top >= buffer_size) {
			buffer_size += 256;
			buffer = realloc(buffer, buffer_size);
		}
	}

	if (c == '\n' || buffer_top > 195) {
		buffer[buffer_top++] = '\0';
		_push_line(buffer);
		buffer = NULL;
	}

	return 0;
}

char fbterm_getch(void) {
	char c;

	if (!_line) {
		_line = _pop_line();
		line_pos = 0;
	}

	c = _line[line_pos];
	line_pos++;

	if (c == '\n') {
		free(_line);
		_line = NULL;
	}

	return c;
}

uint32_t fgjob_pid = 0;

void keyboard_event(uint32_t code, bool press) {
	char c;
	static bool ctrl = false;

	if (!press) {
		ctrl = false;
		return;
	}

	if (code & 0x00800000) {
		if (code == 0x00800001) {
			ctrl = true;
		}
		return;
	}

	c = code;

	if (tolower(c) == 'c' && ctrl) {
		if (fgjob_pid) {
			kill(-fgjob_pid, SIGINT);
		}

		fbterm_print('^');
		fbterm_print('C');

		fbterm_print('\n');
		fbterm_buffer('\n');
		return;
	}

	fbterm_buffer(c);

	if (isprint(c)) {
		fbterm_print(c);
		screen_flip();
	}
}
