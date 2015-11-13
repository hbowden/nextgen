

/**
 * Copyright (c) 2015, Harrison Bowden, Secure Labs, Minneapolis, MN
 * 
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice 
 * and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/

#ifndef CRYPTO_H
#define CRYPTO_H

/* Pass either CRYPTO or NO_CRYPTO to setup_crypto_module().
   When NO_CRYPTO is passed then the crypto module use's non
   cryptographic random number generators.  */
enum crypto_method {CRYPTO, NO_CRYPTO};

/* Call this once to setup the crypto module. */
extern int setup_crypto_module(enum crypto_method method);

/* Use this function to see if were using a software PRNG or hardware.
   This function is useful for knowing when we need to seed the PRNG on forks. */
extern int using_hardware_prng(void);

/* This function seeds the software PRNG. */
extern int seed_prng(void);

/* Create a random number in the range set by the variable range.  */
extern int rand_range(unsigned int range, unsigned int *number);

/* Fill the buffer with length random bytes. */
extern int rand_bytes(char **buf, unsigned int length);

/* Fill the buffer out with the sha256 hash of in. */
extern int sha256(char *in, char **out);

/* Fill the buffer out with the sha512 hash of in. */
extern int sha512(char *in, char **out);

#endif /* End of header file. */
