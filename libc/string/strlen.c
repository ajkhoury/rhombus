/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>

size_t strlen(const char *s) {
	size_t i;

	for (i = 0; s[i] != '\0'; i++);

	return i;
}
