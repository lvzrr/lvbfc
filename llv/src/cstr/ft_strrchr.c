#include "cstr.h"

char	*lv_strrchr(const char *haystack, int needle)
{
	t_u8	*l_o;
	size_t	s;

	if (!haystack)
		return (NULL);
	l_o = NULL;
	s = lv_strlen(haystack);
	if (needle == '\0')
		return ((char *)&(haystack[s]));
	while (s--)
		if (haystack[s] == (char)needle)
			return ((char *)&(haystack[s]));
	return ((char *)l_o);
}
