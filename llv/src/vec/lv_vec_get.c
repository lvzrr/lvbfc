#include "vec.h"

const void	*lv_vec_get(t_vec *vec, size_t idx)
{
	t_u8	*raw;

	if (!vec || !vec->size || idx > vec->size)
		return (NULL);
	raw = (t_u8 *)vec->data;
	return (raw + (vec->sizeof_type * idx));
}

void	*lv_vec_get_mut(t_vec *vec, size_t idx)
{
	t_u8	*raw;

	if (!vec || !vec->size || idx > vec->size)
		return (NULL);
	raw = (t_u8 *)vec->data;
	return (raw + (vec->sizeof_type * idx));
}

void	*lv_vec_get_clone(t_vec *vec, size_t idx)
{
	t_u8	*raw;

	if (!vec || !vec->size || idx > vec->size)
		return (NULL);
	raw = (t_u8 *)vec->data;
	return (lv_memclone(raw + (vec->sizeof_type * idx),
			vec->sizeof_type));
}
