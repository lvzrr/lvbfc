#include "vec.h"

void	lv_vec_clear(t_vec *vec)
{
	if (!vec || !vec->data)
		return ;
	lv_memset(vec->data, 0, vec->size * vec->sizeof_type);
	vec->size = 0;
}
