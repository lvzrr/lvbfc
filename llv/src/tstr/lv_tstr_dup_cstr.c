#include "llv.h"

char	*lv_tstr_dup_cstr(t_string *str)
{
	if (!str || !str->data)
		return (NULL);
	return (lv_memclone(str->data, str->alloc_size));
}
