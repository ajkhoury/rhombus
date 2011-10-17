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

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <proc.h>

/****************************************************************************
 * _sigpanic
 *
 * Print an error message and abort.
 */

static void _sigpanic(const char *message) {

	_stop(-1);

	fprintf(stderr,
		"%s (pid %d \"%s\" terminated)\n", message, getpid(), getname_s());

	abort();
}

/****************************************************************************
 * __sig_default
 *
 * Default signal handler. Prints error messages and aborts for certain
 * signals; does nothing for others.
 */

void __sig_default(int signum) {
	
	switch (signum) {
	case SIGQUIT:	_sigpanic("Termination request");		break;
	case SIGTERM:	_sigpanic("Termination request");		break;
	case SIGABRT:	_sigpanic("Abnormal termination");		break;
	case SIGKILL:	_sigpanic("Process killed");			break;
	case SIGINT:	_sigpanic("Process interrupted");		break;
	case SIGFPE:	_sigpanic("Floating point exception");	break;
	case SIGSEGV:	_sigpanic("Segmentation fault");		break;
	case SIGILL:	_sigpanic("Illegal operation");			break;
	}
}
