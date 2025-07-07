#ifndef VEC_H
# define VEC_H

# include <stddef.h>
# include "structs.h"
# include "mem.h"
# include "macros.h"

t_vec		lv_vec(size_t alloc_size, size_t sizeof_type);
void		lv_vec_push(t_vec *vec, void *data, size_t len);
void		lv_vec_free(t_vec *vec);
void		lv_vec_reserve(t_vec *v, size_t n);
void		lv_vec_pop(t_vec *__restrict__ v);
void		lv_vec_clear(t_vec *vec);
void		lv_vec_fit(t_vec *vec);
void		lv_vec_insert(t_vec *v, size_t index, void *data,
				size_t len);
void		*lv_vec_peek_last(t_vec *__restrict__ v);
t_u8		lv_vec_popmv(void *__restrict__ dst, t_vec *__restrict__ v);
void		lv_vec_rev(t_vec *v);
void		*lv_vec_get_mut(t_vec *vec, size_t idx);
void		*lv_vec_get_clone(t_vec *vec, size_t idx);
const void	*lv_vec_get(t_vec *vec, size_t idx);
#endif
