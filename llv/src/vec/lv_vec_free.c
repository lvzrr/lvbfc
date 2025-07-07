#include "vec.h"

void	lv_vec_free(t_vec *v)
{
	void	*tmp;

	if (!v)
		return ;
	tmp = (void *)v->data;
	lv_free(&tmp);
	v->data = NULL;
	v->size = 0;
	v->alloc_size = 0;
	v->sizeof_type = 0;
}
