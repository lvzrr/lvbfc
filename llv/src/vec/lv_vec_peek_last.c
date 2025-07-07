#include "vec.h"

inline void	*lv_vec_peek_last(t_vec *__restrict__ v)
{
	if (!v)
		return (NULL);
	if (v->size == 0)
		return (v->data);
	return ((t_u8 *)v->data + (v->size - 1)
		* v->sizeof_type);
}
