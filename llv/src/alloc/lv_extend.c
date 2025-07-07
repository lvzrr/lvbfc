#include "alloc.h"

void	*lv_extend(void *ptr, size_t n, size_t size)
{
	unsigned char	*p2;

	if (size > SIZE_MAX - n || (size == 0 && ptr))
		return (lv_free(&ptr), NULL);
	if (!ptr)
		return (lv_alloc(n + size));
	p2 = lv_alloc(n + size);
	if (!p2)
		return (NULL);
	lv_memmove(p2, ptr, n);
	return (lv_free(&ptr), p2);
}
