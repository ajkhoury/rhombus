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

#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * event
 *
 * Asynchronous event protocol. Sends an 8-byte value to the given resource,
 * without waiting for a reply. Returns zero on success, nonzero on error.
 *
 * protocol:
 *   port: PORT_EVENT
 *
 *   event:
 *     uint64_t value
 */

int event(uint64_t rp, uint64_t value) {
	struct msg *msg;

	msg = aalloc(sizeof(struct msg) + sizeof(uint64_t), PAGESZ);
	if (!msg) return 1;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = rp;
	msg->length = sizeof(uint64_t);
	msg->port   = PORT_EVENT;
	msg->arch   = ARCH_NAT;
	((uint64_t*) msg->data)[0] = value;

	return msend(msg);
}

/******************************************************************************
 * eventl
 *
 * Sends an event to every resource on the event list <list>. Returns zero on 
 * success, nonzero on error.
 */

int eventl(struct event_list *list, uint64_t value) {
	
	while (list) {
		event(list->target, value);
		list = list->next;
	}

	return 0;
}

static event_handler_t _event_handler;
static void _event_wrapper(struct msg *msg);

int event_register(uint64_t source, event_handler_t handler) {
	char *reply;

	if (handler) {
		reply = rcall(source, "register");

		if (!strcmp(reply, "T")) {
			when(PORT_EVENT, _event_wrapper);
			_event_handler = handler;
			free(reply);
			return 0;
		}
		else {
			free(reply);
			return 1;
		}
	}
	else {
		reply = rcall(source, "deregister");

		if (!strcmp(reply, "T")) {
			free(reply);
			return 0;
		}
		else {
			free(reply);
			return 1;
		}
	}
}

static void _event_wrapper(struct msg *msg) {
	uint64_t value;

	if (msg->length != sizeof(uint64_t)) {
		return;
	}

	if (!_event_handler) {
		return;
	}

	value = ((uint64_t*) msg->data)[0];

	_event_handler(msg->source, value);
}
