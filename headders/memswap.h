#ifndef __MEMSWAP_H__
#define __MEMSWAP_H__

#include <stdint.h>
/**
 * @brief Fucntion swaps data at 2 non aliasing mem locations
 * 
 * @param a: swap target
 * @param b: swap target
 * @param size: size of memory to swap
 */
static inline void memswap(void  *restrict a, void *restrict b, uint32_t size)
{
	uint8_t temp_byte;
	uint8_t *a_ptr = (uint8_t *)a;
	uint8_t *b_ptr = (uint8_t *)b;

	while (size--)
	{
		temp_byte = *a_ptr;
		*a_ptr++ = *b_ptr;
		*b_ptr++ = temp_byte;
	}
}

#endif