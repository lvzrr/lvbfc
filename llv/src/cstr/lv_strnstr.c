#include "cstr.h"

char	*lv_strnstr(const char *haystack, const char *needle, size_t n)
{
	size_t	i;
	size_t	l;

	if (!needle || !*needle)
		return ((char *)haystack);
	if (!haystack || !*haystack)
		return (NULL);
	i = 0;
	l = lv_strlen(needle);
	if (l > n)
		return (NULL);
	while (haystack[i] && (i + l) <= n)
	{
		if (!lv_strncmp(&(haystack[i]), needle, l))
			return ((char *)(&(haystack[i])));
		i++;
	}
	return ((void *) 0);
}
