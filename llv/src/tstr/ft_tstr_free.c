#include "llv.h"

void	lv_tstr_free(t_string *str)
{
	void	*tmp;

	if (str->data)
	{
		tmp = str->data;
		lv_free(&tmp);
		str->data = NULL;
	}
	str->len = 0;
	str->alloc_size = 0;
}
