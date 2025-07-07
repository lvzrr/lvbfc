#include "llv.h"

t_string	lv_tstr_new(size_t len)
{
	t_string	out;

	out.len = 0;
	out.alloc_size = (size_t) len + 1;
	out.data = lv_calloc(out.alloc_size, 1);
	if (!out.data)
		out = (t_string){0};
	return (out);
}
