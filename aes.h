#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>
#include "bs.h"

void aes_ecb_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key);
void aes_ecb_decrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key);

void aes_ctr_encrypt_ref(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv, word_t (* rk)[BLOCK_SIZE]);
#define aes_ctr_decrypt_ref(outputb,inputb,size,key,iv, rk) aes_ctr_encrypt(outputb,inputb,size,key,iv, rk)

void aes_ctr_encrypt(uint8_t * outputb, uint8_t * inputb, int size, uint8_t * key, uint8_t * iv, word_t * rk);
#define aes_ctr_decrypt(outputb,inputb,size,key,iv,rk) aes_ctr_encrypt(outputb,inputb,size,key,iv,rk)

void shift_rk(word_t * rk, int shift);

#endif
