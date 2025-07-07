#include "alloc.h"

void	*lv_realloc(void *ptr, size_t n, size_t size)
{
	void	*p;

	if (!size)
		return (lv_free(&ptr), NULL);
	if (!ptr)
		return (lv_alloc(size));
	p = lv_alloc(size);
	if (!p)
		return (lv_free(&ptr), NULL);
	if (size < n)
		n = size;
	lv_memmove(p, ptr, n);
	if (ptr)
		lv_free(&ptr);
	return (p);
}
