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

#ifndef __RLIBC_EXEC_H
#define __RLIBC_EXEC_H

#include <stdint.h>
#include <stddef.h>
#include <dlfcn.h>

/* executable loading *******************************************************/

void *load_exec  (const char *name);
void *load_shared(const char *soname);

/* path resolution **********************************************************/

char *path_resolve  (const char *file);
char *ldpath_resolve(const char *soname);

/* execute ******************************************************************/

/* with executable image */
int execive(uint8_t *image, size_t size, char const **argv, char const **envp);
int execiv (uint8_t *image, size_t size, char const **argv);
int execi  (uint8_t *image, size_t size);

/* from filesystem */
int execve (const char *path, char const **argv, char const **envp);
int execv  (const char *path, char const **argv);
int exec   (const char *path);

int execvpe(const char *file, char const **argv, char const **envp);
int execvp (const char *file, char const **argv);

/* things to save on exec ***************************************************/

char  *packarg(const char **argv);
char **loadarg(char *pack);

#endif/*__RLIBC_EXEC_H*/
