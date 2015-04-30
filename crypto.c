

#include "crypto.h"
#include <openssl/engine.h>

void set_up_hardware_acceleration(void)
{
    OPENSSL_cpuid_setup();
    ENGINE_load_rdrand();

    ENGINE* engine = ENGINE_by_id("rdrand");
    if(engine == NULL)
    {
    	printf("No intel random number generator detected\n");
    	return -1;
    }

    /* Note. openssl use's o for it's exit code. */
    int rtrn = ENGINE_init(engine);
    if(rtrn == 0)
    {
        printf("Can't init crypto engine\n");
        return -1;
    }

    rtrn = ENGINE_set_default(engine, ENGINE_METHOD_RAND);
    if(rtrn == 0)
    {
        printf("Can't set default method\n");
        return -1;
    }
    
	return;
}