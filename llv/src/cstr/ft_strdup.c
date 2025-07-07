#include "cstr.h"

char	*lv_strdup(const char *str)
{
	size_t	n;

	if (!str)
		return (NULL);
	n = lv_strlen(str);
	return (lv_memclone((void *)str, n + 1));
}
