#include "vec.h"

void	lv_vec_insert(t_vec *v, size_t index, void *data, size_t len)
{
	void	*new;
	size_t	new_alloc;

	if (!v || !data || len == 0 || index > v->size)
		return ;
	if (v->alloc_size < v->size + len)
	{
		new_alloc = __max_s(v->alloc_size * 2, v->size + len);
		new = lv_extend_zero(v->data, v->alloc_size * v->sizeof_type,
				(new_alloc - v->alloc_size) * v->sizeof_type);
		if (!new)
			return ;
		v->data = new;
		v->alloc_size = new_alloc;
	}
	lv_memmove((t_u8 *)v->data + (index + len) * v->sizeof_type,
		(t_u8 *)v->data + index * v->sizeof_type,
		(v->size - index) * v->sizeof_type);
	lv_memcpy((t_u8 *)v->data + index * v->sizeof_type,
		data, len * v->sizeof_type);
	v->size += len;
}
