#include "cstr.h"

char	*lv_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if (*s == c)
			return ((char *)s);
		s++;
	}
	if (!c && !*s)
		return ((char *)s);
	return (NULL);
}
