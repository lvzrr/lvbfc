#include "mem.h"

void	lv_bzero(void *__restrict__ ptr, size_t n)
{
	lv_memset(ptr, 0, n);
}
