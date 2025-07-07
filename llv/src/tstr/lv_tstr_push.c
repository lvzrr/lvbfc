#include "llv.h"

void	lv_tstr_push(t_string *str, char c)
{
	if (!str)
		return ;
	if (str->len + 1 < str->alloc_size)
	{
		str->data[str->len++] = c;
		str->data[str->len] = 0;
		return ;
	}
	str->data = lv_extend_zero(str->data, str->alloc_size, str->alloc_size * 2);
	str->alloc_size *= 2;
	if (!str->data)
		return ;
	str->data[str->len++] = c;
	str->data[str->len] = 0;
}
