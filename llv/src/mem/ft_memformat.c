#include "mem.h"

void	*lv_memformat(void *ptr, size_t size)
{
	void	*fmt;

	if (!ptr || !size)
		return (NULL);
	fmt = lv_memclone(ptr, size);
	return (free(ptr), fmt);
}
