#ifndef __HASHES_H__
#define __HASHES_H__
#include <stdint.h>
#include "string_buffer.h"
#include<string.h>
// -----------------------------------------------------------------------------
// MurmurHash3 32-bit (x86 variant), public domain by Austin Appleby.
// Adapted here for hashing strings of known length with a fixed seed.
// https://github.com/aappleby/smhasher
// ----------------------------------------------------------------------

static inline uint32_t murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51u;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593u;
    return k;
}

uint32_t murmur3_32(const char *key, uint32_t len) {
    const char *data = key;
    uint32_t h = 0; // fixed seed
    uint32_t k;

    // Process 4-byte blocks
    size_t nblocks = len / 4;
    for (uint32_t i = 0; i < nblocks; i++) {
        memcpy(&k, data, sizeof(uint32_t));
        data += 4;
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64u;
    }

    // Tail: remaining bytes
    k = 0;
    switch (len & 3) {
        case 3: k ^= data[2] << 16; //fallthrough
        case 2: k ^= data[1] << 8;	//fallthrough
        case 1: k ^= data[0];		//fallthrough
                h ^= murmur_32_scramble(k);
    }

    // Finalization
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6bU;
    h ^= h >> 13;
    h *= 0xc2b2ae35U;
    h ^= h >> 16;

    return h;
}

/**
 * @brief function that hashs a key
 * 
 * @par Function for now just calls the implementation of murmur3 hash function
 * 
 * @param key: key to hash
 * 
 * @returns hash of a key
 */
static inline uint32_t get_hash(const StringView *key)
{
	return murmur3_32(key->data,key->length);
}

#endif