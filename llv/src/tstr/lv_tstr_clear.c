#include "tstr.h"

void	lv_tstr_clear(t_string *s)
{
	if (!s || !s->data)
		return ;
	lv_memset(s->data, 0, s->len);
	s->len = 0;
}
