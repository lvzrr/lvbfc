#include "tstr.h"

char	lv_tstr_pop(t_string *str)
{
	char	o;

	if (!str || !str->data || !str->len)
		return (0);
	o = lv_tstr_borrow(str)[str->len - 1];
	str->data[str->len - 1] = 0;
	str->len--;
	return (o);
}
