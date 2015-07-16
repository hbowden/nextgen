

#include "crypto.h"
#include "utils.h"
#include "nextgen.h"
#include <errno.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/engine.h>

static int (*rand_range_pointer)(unsigned int range, int *number);

static int rand_range_no_crypto(unsigned int range, int *number)
{
	*number = (int) rand() % range + 1;
	return 0;
}

static int rand_range_crypto(unsigned int range, int *number)
{
	BIGNUM *random, *range1;

    random = BN_new();
    if(random == NULL)
    {
    	output(ERROR, "Can't init bignum struct\n");
    	return -1;
    }

    range1 = BN_new();
    if(random == NULL)
    {
    	output(ERROR, "Can't init bignum struct\n");
    	return -1;
    }

    int rtrn = BN_set_word(range1, range);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't set range\n");
    	return -1;
    }

    rtrn = BN_rand_range(random, range1);
    if(rtrn < 0)
    {
    	output(ERROR, "Can't get random range\n");
    	return -1;
    }

    char *buf = BN_bn2dec(random);
    if(buf == NULL)
    {
    	output(ERROR, "Can't convert random number\n");
    	return -1;
    }

    *number = (unsigned int)strtol(buf, NULL, 10);
    if(*number > (int) range || *number < 0)
    {
    	output(ERROR, "Can't convert to int.");
    	return -1;
    }

	return 0;
}

static int setup_rand_range(char *method)
{
    if(strncmp(method, "no-crypto", 10) == 0)
    {
        rand_range_pointer = &rand_range_no_crypto;
    }
    else if(strncmp(method, "crypto", 6) == 0)
    {
    	rand_range_pointer = &rand_range_crypto;
    }
    else
    {
    	output(ERROR, "Bad argument to crypto option, try again\n");
    	return -1;
    }
	return 0;
}

int rand_range(unsigned int range, int *number)
{
    return rand_range_pointer(range, number);
}

static int setup_hardware_acceleration(void)
{
#ifndef MAC_OSX
    ENGINE_load_rdrand();
#endif

    ENGINE* engine = ENGINE_by_id("rdrand");
    if(engine == NULL)
    {
    	output(STD, "No intel random number generator detected\n");
    	return 1;
    }

    /* Note. openssl use's zero for it's exit code. */
    int rtrn = ENGINE_init(engine);
    if(rtrn == 0)
    {
        output(ERROR, "Can't init crypto engine\n");
        return -1;
    }

    rtrn = ENGINE_set_default(engine, ENGINE_METHOD_RAND);
    if(rtrn == 0)
    {
        printf("Can't set default method\n");
        return -1;
    }
    
	return 0;
}

static int seed_prng(void)
{
    
    return 0;
}

int setup_crypto(void)
{
	int rtrn;

	if(map->crypto_flag == TRUE)
	{
        rtrn = setup_rand_range(map->crypto_method);
        if(rtrn < 0)
        {
        	output(ERROR,"Can't setup random range function.\n");
        	return -1;
        }

        if(strncmp(map->crypto_method, "crypto", 6) == 0)
        {
        	rtrn = setup_hardware_acceleration();
        	switch(rtrn)
        	{
        		case 0: output(STD, "Using hardware crypto accelerator\n"); break;

        		case 1: output(STD, "Switching to /dev/urandom \n"); return seed_prng();

        		default: output(ERROR, "Error while trying to setup hardware acceleration\n"); return -1;
        	}
        }  
	}
	else
	{
		rtrn = setup_rand_range((char *)"crypto");
        if(rtrn < 0)
        {
        	output(ERROR,"Can't setup random range function.\n");
        	return -1;
        }

        rtrn = setup_hardware_acceleration();
        switch(rtrn)
        {
        	case 0: output(STD, "Using hardware crypto accelerator\n"); break;

        	case 1: output(STD, "Switching to /dev/urandom \n"); return seed_prng();

        	default: output(ERROR, "Error while trying to setup hardware acceleration\n"); return -1;
        }  
	}
	return 0;
}