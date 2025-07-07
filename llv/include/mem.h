#ifndef MEM_H
# define MEM_H
# include <sys/types.h>
# include <stdlib.h>
# include <limits.h>
# include "alloc.h"
# include "structs.h"
# include "macros.h"

# define LONES_64 0x0101010101010101ULL
# define HIGHS_64 0x8080808080808080ULL
# define ZEROS_64 0x0000000000000000ULL
# define LONES_32  0x01010101U
# define HIGHS_32  0x80808080U

// Actual api

void			lv_bzero(void *__restrict__ ptr, size_t n);
void			*lv_memset(void *__restrict__ s, int c, size_t n);
void			*lv_memcpy(void *__restrict__ dest,
					const void *__restrict__ src, size_t n);
void			lv_memtake(void *__restrict__ dest,
					void *__restrict__ src, size_t n);
t_u8			lv_memswap(void *__restrict__ p1,
					void *__restrict__ p2, size_t n);
t_u8			lv_memswap_extern(void *__restrict__ p1,
					void *__restrict__ p2, size_t n,
					void *__restrict__ buf);
void			*lv_memmove(void *__restrict__ dest,
					const void *__restrict__ src, size_t n);
void			*lv_memchr(const void *__restrict__ ptr, int c, size_t n);
ssize_t			lv_memcmp(void *__restrict__ dest,
					const void *__restrict__ src, size_t n);
void			*lv_memclone(void *__restrict__ ptr, size_t size);
void			*lv_memformat(void *ptr, size_t size);
void			lv_membroadcast(void *dst, void *src, size_t chunks, size_t n);

/*
 *
 *	HELPERS
 *
 */

// COPY 
void			_copy_u8_fwd(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u32_fwd(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u64_fwd(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u128_fwd(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u8_bw(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u32_bw(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u64_bw(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
void			_copy_u128_bw(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);

// OVERWRITING

void			_write_u8_fwd(void **__restrict__ dest,
					t_u8 x,
					size_t *__restrict__ n);
void			_write_u32_fwd(void **__restrict__ dest,
					t_u32 x,
					size_t *__restrict__ n);
void			_write_u64_fwd(void **__restrict__ dest,
					t_u64 x,
					size_t *__restrict__ n);
void			_write_u128_fwd(void **__restrict__ dest,
					t_u128 x,
					size_t *__restrict__ n);
// CMP

ssize_t			_cmp_u8(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
ssize_t			_cmp_u32(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
ssize_t			_cmp_u64(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);
ssize_t			_cmp_u128(void **__restrict__ dest,
					const void **__restrict__ src,
					size_t *__restrict__ n);

// LIL' HELPERS

int				_lookup_u32(t_u32 word, t_u32 mask);
int				_lookup_u64(t_u64 word, t_u64 mask);
int				_lookup_u128(t_u128 word, t_u128 mask);
int				__hasz128(t_u128 x);
int				__hasz64(t_u64 x);
t_u128			__populate(t_u8 y);

// ALIGNMIENT & CHECKZ
t_u8			lv_memctz_u32(t_u32 x);
t_u8			lv_memctz_u64(t_u64 x);
t_u8			lv_memctz_u128(t_u128 x);

size_t			__max_s(size_t x, size_t y);
t_u8			__maxu8(t_u8 x, t_u8 y);
t_u32			__maxu32(t_u32 x, t_u32 y);
t_u64			__maxu64(t_u64 x, t_u64 y);
t_u128			__maxu128(t_u128 x, t_u128 y);
#endif
