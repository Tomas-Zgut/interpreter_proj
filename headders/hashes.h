#ifndef __HASHES_H__
#define __HASHES_H__
#include <stdint.h>
#include "string_buffer.h"
#include<string.h>
// -----------------------------------------------------------------------------
// MurmurHash3 64-bit, public domain by Austin Appleby.
// Adapted here for hashing strings of known length with a fixed seed.
// https://github.com/aappleby/smhasher
// ----------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline

#include <stdlib.h>

#define ROTL64(x,y)	_rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

static inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
    return (x << r) | (x >> (64 - r));
}

#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

FORCE_INLINE uint64_t getblock64 ( const uint64_t * p, int i )
{
  return p[i];
}

#define FIXED_SEED 0

static inline uint64_t MurmurHash3_x64_128 (const void * key, const int len)
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint64_t h1 = FIXED_SEED;
  uint64_t h2 = FIXED_SEED;

  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(int i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock64(blocks,i*2+0);
    uint64_t k2 = getblock64(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= ((uint64_t)tail[14]) << 48; //fallthrough
  case 14: k2 ^= ((uint64_t)tail[13]) << 40; //fallthrough
  case 13: k2 ^= ((uint64_t)tail[12]) << 32; //fallthrough
  case 12: k2 ^= ((uint64_t)tail[11]) << 24; //fallthrough
  case 11: k2 ^= ((uint64_t)tail[10]) << 16; //fallthrough
  case 10: k2 ^= ((uint64_t)tail[ 9]) << 8; //fallthrough
  case  9: k2 ^= ((uint64_t)tail[ 8]) << 0; 
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2; //fallthrough

  case  8: k1 ^= ((uint64_t)tail[ 7]) << 56; //fallthrough
  case  7: k1 ^= ((uint64_t)tail[ 6]) << 48; //fallthrough
  case  6: k1 ^= ((uint64_t)tail[ 5]) << 40; //fallthrough
  case  5: k1 ^= ((uint64_t)tail[ 4]) << 32; //fallthrough
  case  4: k1 ^= ((uint64_t)tail[ 3]) << 24; //fallthrough
  case  3: k1 ^= ((uint64_t)tail[ 2]) << 16; //fallthrough
  case  2: k1 ^= ((uint64_t)tail[ 1]) << 8; //fallthrough
  case  1: k1 ^= ((uint64_t)tail[ 0]) << 0; //fallthrough
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1; //fallthrough
  };

  //----------
  // finalization

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  return h2;
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
static inline uint64_t get_hash(const StringView *key)
{
	return MurmurHash3_x64_128(key->data,key->length);
}

#endif