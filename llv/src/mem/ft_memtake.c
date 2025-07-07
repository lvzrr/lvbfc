#include "vec.h"

void	lv_memtake(void *__restrict__ dst,
	void *__restrict__ src, size_t len)
{
	if (!dst || !src || !len)
		return ;
	lv_memcpy(dst, src, len);
	lv_bzero(src, len);
}
