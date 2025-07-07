#include "tstr.h"

void	lv_tstr_fit(t_string *str)
{
	char	*new;

	if (!str || str->len + 1 == str->alloc_size)
		return ;
	new = lv_realloc(str->data, str->alloc_size, str->len + 1);
	if (!new)
		return ;
	str->data = new;
	str->alloc_size = str->len + 1;
}
