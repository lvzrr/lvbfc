#include "vec.h"

void	lv_vec_reserve(t_vec *v, size_t n)
{
	void	*new;

	if (!v || !n || n <= v->alloc_size)
		return ;
	new = lv_extend_zero(v->data, v->alloc_size * v->sizeof_type,
			n * v->sizeof_type);
	if (!new)
		return ;
	v->data = new;
	v->alloc_size += n;
}
