/*#include <stdio.h>*/
#include <stdlib.h>
#include <string.h>
#include "bs.h"
#include "aes.h"
#include "utils.h"

#ifdef RUN_TESTS
#include "tests/tests.h"
#endif

#ifdef TEST_FOOTPRINT
#define printf(fmt, ...) (0)
#define fprintf(f,fmt, ...) (0)
#else
#include <stdio.h>
#endif



// TODO AES CTR
int main(int argc, char * argv[])
{

#ifdef RUN_TESTS
#ifndef TEST_FOOTPRINT
    aes_ecb_test();
#endif
    aes_ctr_test();
    return 0;
#endif

    if (argc < 2)
    {
        fprintf(stderr,"usage: %s <input-file>\n", argv[0]);
        exit(1);
    }
    return 0;
}


