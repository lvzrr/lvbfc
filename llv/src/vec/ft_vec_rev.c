#include "vec.h"

void	lv_vec_rev(t_vec *v)
{
	size_t	half;
	size_t	i;

	if (!v || v->size < 2)
		return ;
	half = v->size / 2;
	i = 0;
	while (i < half)
	{
		lv_memswap(
			(t_u8 *)v->data + (v->sizeof_type * i),
			(t_u8 *)v->data + (v->sizeof_type * (v->size - i - 1)),
			v->sizeof_type);
		i++;
	}
}
