#include <stdint.h>
#include <stdlib.h>
#include "../utils.h"
#include "../aes.h"

#ifdef TEST_FOOTPRINT
#define printf(fmt, ...) (0)
#define fprintf(f,fmt, ...) (0)
#else
#include <stdio.h>
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

    uint8_t pt_vector[20000];
    uint8_t ct_vector[20000];
    uint8_t output[sizeof(ct_vector)];
    uint8_t input[sizeof(pt_vector)];

    FILE * rng = fopen("/dev/urandom","r");

    if (read(fileno(rng), pt_vector, sizeof(pt_vector)) != sizeof(pt_vector))
    {
        perror("read");
        exit(2);
    }


    printf("AES CTR\n");

    printf("key: ");
    dump_hex(key_vector,16);
    printf("iv: ");
    dump_hex(iv_vector, 16);

    aes_ctr_encrypt_fr(output, pt_vector, sizeof(pt_vector), key_vector, iv_vector);
    
    aes_ctr_encrypt(ct_vector, pt_vector, sizeof(pt_vector), key_vector, iv_vector);


    /*printf("cipher text: \n");*/
    /*dump_hex(output,sizeof(pt_vector));*/
    
    aes_ctr_decrypt_fr(input,output,sizeof(pt_vector),key_vector, iv_vector);

    /*printf("plain text: \n");*/
    /*dump_hex(input,sizeof(pt_vector));*/

    if (memcmp(pt_vector, input, sizeof(pt_vector)) != 0)
    {
        fprintf(stderr,"error: decrypted ciphertext is not the same as the input plaintext\n");
        exit(1);
    }
    else if (memcmp(ct_vector, output, sizeof(pt_vector)) != 0)
    {
        fprintf(stderr,"error: ciphertext is not the same as the test vector\n");
        exit(1);
    }
    else
    {
        printf("CTR passes test vector\n\n");
    }

}


