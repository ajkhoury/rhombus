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

#ifndef __RLIBC_PROC_H
#define __RLIBC_PROC_H

#include <rho/abi.h>

/* constants ***************************************************************/

#define MAX_PID     1024
#define MAX_THREADS 256

/* process management ******************************************************/

int fork(void);		/* Spawn new process */

int daemonize(void); /* Play dead and end main thread */

/* thread management *******************************************************/

void     stop  (void);               /* block current thread */
void     freeze(uint32_t tid);       /* block other thread */
void     wake  (uint32_t tid);       /* unblock thread by ID */
uint32_t also  (void (*func)(void)); /* spawn new thread */
void     sleep (void);               /* relinquish timeslice */
void     done  (void);               /* end current thread */

/* kernel time *************************************************************/

/* note: all times are in 256ths of a second since bootup */
uint64_t getktime(void); /* kernel time */
uint64_t getctime(void); /* cpu time */
uint64_t getptime(void); /* process time */
uint64_t getttime(void); /* thread time */

/* PIDs ********************************************************************/

void      resetpid(void);

uint32_t  getpid (void);
uint32_t  getppid(void);
int       setppid(uint32_t ppid);
uint32_t  gettid (void);
uintptr_t gettls (void);

uint32_t  getparent(uint32_t pid);
int       setparent(uint32_t pid, uint32_t ppid);

/* users and authorization *************************************************/

uint32_t  getuser(uint32_t pid);
int       setuser(uint32_t pid, uint32_t user);

uint32_t  getuid (void);
int       setuid (uint32_t user);

/* process names ***********************************************************/

int      setname(const char *name);
char    *getname(uint32_t pid);
char    *getname_s();

/* exit status flags *******************************************************/

#define EXIT_ABORT  0x00010000
#define EXIT_SIGNAL 0x00020000
#define EXIT_DUMPED 0x00040000
#define EXIT_STOP   0x00080000

#endif/*__RLIBC_PROC_H*/
