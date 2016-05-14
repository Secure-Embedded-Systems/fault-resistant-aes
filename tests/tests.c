#include <stdint.h>
#include <stdlib.h>
#include "../aes.h"

#ifdef TEST_FOOTPRINT
#define printf(fmt, ...) (0)
#define fprintf(f,fmt, ...) (0)
#define dump_hex(f,f2)  (0)
#else
#include <stdio.h>
#include "../utils.h"
#endif

void aes_ecb_test()
{
    uint8_t key_vector[16] = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
    uint8_t pt_vector[16] =  "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a";
    uint8_t ct_vector[16] =  "\x3a\xd7\x7b\xb4\x0d\x7a\x36\x60\xa8\x9e\xca\xf3\x24\x66\xef\x97";
    uint8_t output[16];
    uint8_t input[16];
    
    printf("AES ECB\n");

    aes_ecb_encrypt(output, pt_vector,16,key_vector);


    printf("cipher text: \n");
    dump_hex(output, 16);

    aes_ecb_decrypt(input, output, 16, key_vector);

    printf("plain text: \n");
    dump_hex((uint8_t * )input,16);

    if (memcmp(pt_vector, input, 16) != 0)
    {
        fprintf(stderr,"error: decrypted ciphertext is not the same as the input plaintext\n");
        exit(1);
    }
    else if (memcmp(ct_vector, output, 16) != 0)
    {
        fprintf(stderr,"error: ciphertext is not the same as the test vector\n");
        exit(1);
    }
    else
    {
        printf("ECB passes test vector\n\n");
    }
}

void aes_ctr_test()
{
// Test vector from NIST for 4 input blocks

    uint8_t key_vector[16] =
        "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";

    uint8_t iv_vector[16]  =
        "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

#ifndef TEST_FOOTPRINT
#define vector_size 1024
#else
#define vector_size 32
#endif

    uint8_t pt_vector[vector_size];
    uint8_t ct_vector[vector_size];
    uint8_t output[sizeof(ct_vector)+32];
    uint8_t input[sizeof(pt_vector)+32];

#define FILL_RANDOM
#ifdef FILL_RANDOM

    FILE * r = fopen("/dev/urandom","r");
    read(fileno(r), pt_vector, vector_size);
    fclose(r);
    printf("randomized %d bytes of input\n", vector_size);

#endif

    word_t rk[11][BLOCK_SIZE];
    bs_expand_key(rk, key_vector);
    
    word_t rk_dev[11][BLOCK_SIZE];
    bs_expand_key_dev(rk_dev, key_vector);

    printf("AES CTR\n");

    printf("key: ");
    dump_hex(key_vector,16);
    printf("iv: ");
    dump_hex(iv_vector, 16);

    aes_ctr_encrypt_fr(output, pt_vector, sizeof(pt_vector), key_vector, iv_vector, rk_dev);
    
#ifndef TEST_FOOTPRINT
    aes_ctr_encrypt(ct_vector, pt_vector, sizeof(pt_vector), key_vector, iv_vector, rk);
#endif

    aes_ctr_decrypt_fr(input,output,sizeof(pt_vector),key_vector, iv_vector, rk_dev);

#ifndef TEST_FOOTPRINT
    if (memcmp(pt_vector, input, sizeof(pt_vector)) != 0)
    {
        fprintf(stderr,"error: decrypted ciphertext is not the same as the input plaintext\n");
        exit(1);
    }
    else if (memcmp(ct_vector, output, sizeof(pt_vector)) != 0)
    {
        fprintf(stderr,"error: ciphertext is not the same as the test vector\n");
        printf("correct:\n");
        dump_hex(ct_vector,vector_size);
        printf("wrong:\n");
        dump_hex(output,vector_size);
        exit(1);
    }
    else
    {
        printf("CTR passes test vector\n\n");
    }
#endif

}


