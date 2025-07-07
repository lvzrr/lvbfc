#include "vec.h"

void	lv_vec_fit(t_vec *v)
{
	void	*data;

	if (!v || !v->size || !v->alloc_size)
		return ;
	data = lv_realloc(v->data, v->alloc_size * v->sizeof_type,
			v->size * v->sizeof_type);
	if (!data)
		return ;
	v->data = data;
	v->alloc_size = v->size;
}
