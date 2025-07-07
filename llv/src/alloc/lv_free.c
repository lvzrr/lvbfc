#include "alloc.h"

void	lv_free(void **ptr)
{
	void	*pp;

	if (!ptr || !*ptr)
		return ;
	pp = *ptr;
	free(((void **)pp)[-1]);
	*ptr = NULL;
}

void	lv_free_array(void ***arr)
{
	size_t	i;

	if (!arr || !*arr)
		return ;
	i = 0;
	while ((*arr)[i])
		lv_free(&((*arr)[i++]));
	lv_free((void **)arr);
}
