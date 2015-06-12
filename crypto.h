

#ifndef CRYPTO_H
#define CRYPTO_H

#include "private.h"

private extern int setup_crypto(void);

private extern int rand_range(unsigned int range, int *number);

#endif