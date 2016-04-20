#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

void aes_ecb_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key);
void aes_ecb_decrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key);

#endif
