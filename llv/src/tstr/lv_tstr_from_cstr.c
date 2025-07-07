#include "llv.h"

t_string	lv_tstr_from_cstr(const char *str)
{
	t_string	out;

	if (!str)
	{
		out.len = 0;
		out.alloc_size = 1;
		out.data = lv_alloc(1);
		lv_memset(out.data, 0, 1);
	}
	else
	{
		out.len = lv_strlen(str);
		out.alloc_size = out.len + 1;
		out.data = lv_alloc(out.alloc_size);
		lv_memset(out.data, 0, out.alloc_size);
		if (out.data)
			lv_memcpy(out.data, str, out.len);
	}
	return (out);
}
