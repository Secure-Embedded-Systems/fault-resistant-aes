#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <asm-leon/timer.h>

#include "app.h"
#include "../bs.h"

uint16_t feed_crc(uint16_t crc, uint8_t b)
{
    crc ^= b;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
    return crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
}

uint16_t do_crc(uint8_t * data, int len)
{
    int i;
    uint16_t crc = 0;
    for (i=0; i< len; i++)
    {
        crc = feed_crc(crc,data[i]);
    }
    return crc;
}

int cli_app(int argc, char * argv[])
{
    uint8_t key[16] = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
    uint8_t iv[16]  = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    uint8_t pt[16*32];
    uint8_t ct[16*32];
    uint8_t ct2[16*32];

    memset(pt,0xa5, sizeof(pt));

    int amt = sizeof(pt);

    word_t rk[11][BLOCK_SIZE];
    bs_expand_key(rk, key);

    aes_ctr_encrypt(ct, pt, amt, key, iv, rk);

    printf("{");
    printf("%hx",do_crc(ct,sizeof(ct)));
    printf("}\n");
    exit(0);

    return 0;
}
