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

#include <stdlib.h>
#include <string.h>
#include <mutex.h>

/****************************************************************************
 * TT800S RNG for Flux
 *
 * This implementation based on code based on code based on the description 
 * of the TT800 Twisted GFSR RNG in "Twisted GFSR Generators II" by Makoto 
 * Matsumoto and Y. Kurita. It is a 32 bit RNG with a 2^800 period.
 */

/****************************************************************************
 * rand_state
 *
 * State of the random number generator.
 */

static uint32_t *rand_state = NULL;
static bool rand_mutex;

/****************************************************************************
 * rand_regen
 *
 * Regenerates the internal state of the random number generator, to produce
 * a new set of random numbers.
 */

static void rand_regen(uint32_t *state) {
	size_t i;

	for (i = 0; i < 18; i++) {
		state[i] = 
			state[i + 7] ^ 
			(state[i] >> 1) ^ 
			((state[i] & 1) ? 0x8EBFD028 : 0);
	}

	for (i = 18; i < 25; i++) {
		state[i] =
			state[i - 18] ^
			(state[i] >> 1) ^
			((state[i] & 1) ? 0x8EBFD028 : 0);
	}
}

/****************************************************************************
 * rand_salt
 *
 * Random sequence used to help initialize the random number generator by
 * adding entropy to the sequence generated by a LCG performed on the given
 * seed.
 */

static const uint32_t rand_salt[25] = {
	0x95F24DAB,	0x0B685125, 0xE76CCAE7, 0xAF3EC239,	0x715FAD23, 
	0x24A590AD,	0x69E4B5EF,	0xBF456141,	0x96BC1B7B,	0xA7BDF825, 
	0xC1DE75B7,	0x8858A9C9, 0x3DA87693,	0xB657F9DD,	0xFFDC8A9F,	
	0x8121DA71,	0x8B823ECB,	0x885D05F5, 0x4E20CD47,	0x5A9AD5D9,
	0x512C0C03,	0xEA857CCD,	0x4CC1D30F,	0x8891A9A1,	0xA6B7AADB
};

/****************************************************************************
 * srand
 *
 * Seeds the random number generator. The seed is run through a LCG taken
 * from the Public Domain C Library, xor'ed with the elements of rand_salt,
 * and then placed into the state of the random number generator. The
 * generator state is then regenerated, to minimize nonrandomness produced
 * by the less uniform LCG.
 *
 * If the given seed is 1, the generator is put into the same state as it was 
 * when the library was originally initialized. Or, more precisely, when the
 * random number generator is originally initialized, it is given a seed 
 * of 1.
 */

void srand(uint32_t seed) {

	mutex_spin(&rand_mutex);

	if (rand_state) {
		free(rand_state);
	}

	rand_state = srands(seed);

	mutex_free(&rand_mutex);
}

/****************************************************************************
 * rand
 *
 * Returns a random number in the range 0 to RAND_MAX.
 *
 * Initializes the random number generator with a seed of 1 if it has not
 * yet been initialized. Regenerates the generator's state using rand_regen
 * if all values of the state have been used. Tempers the resulting value to
 * ensure uniform distribution across bits.
 */

uint32_t rand(void) {
	uint32_t value;

	mutex_spin(&rand_mutex);

	if (!rand_state) rand_state = srands(1);
	value = rands(rand_state);

	mutex_free(&rand_mutex);

	return value;
}

/*****************************************************************************
 * memrand
 *
 * Using the random number generator, randomizes the first n bytes of the
 * memory region pointed to by p. The probability of any bit being 1 in that
 * region is 1/2 and independent of all other bits.
 */

void memrand(void *p, size_t n) {

	mutex_spin(&rand_mutex);
	memrands(p, n, rand_state);
	mutex_free(&rand_mutex);
}

/*****************************************************************************
 * randmod
 *
 * Returns a random number in the range from 0 to mod - 1. The result is
 * guaranteed to be uniformly distributed across its range even for large
 * moduli, unlike rand() % mod.
 */

uint32_t randmod(uint32_t n, uint32_t mod) {
	double value;

	value = n;
	value /= RAND_MAX;
	value *= mod;

	return value;
}

/*****************************************************************************
 * srands
 *
 * Creates a random number generator state in dynamically allocated memory
 * using the given seed and returns it. The global random number generator is
 * not affected.
 */

void *srands(uint32_t seed) {
	uint32_t *state;
	size_t i;

	/* allocate state */
	state = malloc(sizeof(uint32_t) * 26);
	if (!state) return NULL;

	/* seed allocator */
	for (i = 0; i < 25; i++) {
		seed = seed * 0x41C64E6D + 0x3039;
		state[i] = seed ^ rand_salt[i];

		/* on the off chance... */
		if (state[i] == 0) {
			state[i] = rand_salt[i];
		}
	}

	rand_regen(state);

	/* set index to 0 */
	state[25] = 0;

	return state;
}

/*****************************************************************************
 * rands
 *
 * Returns a random number in the range 0 to RAND_MAX, using the given random 
 * number generator state.
 */

uint32_t rands(void *state) {
	uint32_t value;
	uint32_t *state32 = state;

	if (!state) return 0;

	if (state32[25] >= 25) {
		rand_regen(state);
		state32[25] = 0;
	}

	value = state32[state32[25]];
	state32[25]++;

	value ^= (value << 7)  & 0x2B5B2500;
	value ^= (value << 15) & 0xDB8B0000;
	value ^= (value >> 16);

	return value;
}

/*****************************************************************************
 * memrands
 *
 * Using the given random number generator state, randomizes the first n bytes 
 * of the memory region pointed to by p. The probability of any bit being 1 in
 * that region is 1/2 and independent of all other bits.
 */

void memrands(void *p, size_t n, void *state) {
	uint8_t *byte = p;
	uint32_t value;
	size_t i;
	
	for (i = 0; i < n; i++) {
		if (i % 4 == 0) {
			value = rands(state);
		}

		byte[i] = value & 0xFF;
		value >>= 8;
	}
}
