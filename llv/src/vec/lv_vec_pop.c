#include "vec.h"

inline void	lv_vec_pop(t_vec *__restrict__ v)
{
	void	*ptr;

	if (!v || !v->data || v->size == 0)
		return ;
	v->size--;
	ptr = (t_u8 *)v->data + (v->size * v->sizeof_type);
	lv_bzero(ptr, v->sizeof_type);
}
