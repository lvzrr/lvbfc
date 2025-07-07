#include "mem.h"

void	*lv_memchr(const void *__restrict__ ptr, int c, size_t n)
{
	unsigned int	i;
	t_u8			*p;

	p = (t_u8 *)ptr;
	i = 0;
	while (i < n)
	{
		if (p[i] == (t_u8)c)
			return (p + i);
		i++;
	}
	return ((void *) 0);
}
