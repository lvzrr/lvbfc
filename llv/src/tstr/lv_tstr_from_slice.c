#include "tstr.h"

t_string	lv_tstr_from_slice(char *s, size_t n)
{
	t_string	out;

	if (!s)
	{
		out.len = 0;
		out.alloc_size = 1;
		out.data = lv_alloc(1);
		lv_memset(out.data, 0, 1);
	}
	else
	{
		out.len = n;
		out.alloc_size = out.len + 1;
		out.data = lv_memclone(s, out.len);
		lv_memset(out.data, 0, out.alloc_size);
		if (out.data)
			lv_memmove(out.data, s, out.len);
	}
	return (out);
}
