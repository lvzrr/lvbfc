#include "alloc.h"

void	*lv_recalloc(void *ptr, size_t n, size_t size)
{
	void	*p;

	if (!size)
		return (lv_free(&ptr), NULL);
	if (!ptr)
		return (lv_calloc(size, 1));
	p = lv_calloc(size, 1);
	if (!p)
		return (lv_free(&ptr), NULL);
	if (size < n)
		n = size;
	lv_memmove(p, ptr, n);
	if (ptr)
		lv_free(&ptr);
	return (p);
}
