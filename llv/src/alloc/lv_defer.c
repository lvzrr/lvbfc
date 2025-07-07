#include "alloc.h"

void	lv_defer(void *p)
{
	lv_free((void **)p);
}

void	lv_defer_arr(void ***p)
{
	lv_free_array((void ***)p);
}
