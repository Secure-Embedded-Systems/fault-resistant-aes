#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>
#include "bs.h"

void aes_ecb_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key);
void aes_ecb_decrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key);

void aes_ctr_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv);
#define aes_ctr_decrypt(outputb,inputb,size,key,iv) aes_ctr_encrypt(outputb,inputb,size,key,iv)

void aes_ctr_encrypt_fr(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv);
#define aes_ctr_decrypt_fr(outputb,inputb,size,key,iv) aes_ctr_encrypt(outputb,inputb,size,key,iv)

word_t fr_get_mask();

#endif
