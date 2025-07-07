#include "cstr.h"

char	*lv_substr(char const *s, unsigned int start, size_t len)
{
	unsigned int	j;
	char			*out;
	size_t			ls;

	if (!s)
		return (NULL);
	ls = lv_strlen(s);
	if (len == 0 || start >= ls)
		return (lv_strdup(""));
	if (len > ls - start)
		len = ls - start;
	out = lv_alloc(len + 1);
	if (!out)
		return (NULL);
	j = 0;
	while (j < len && s[start])
		out[j++] = s[start++];
	out[j] = 0;
	return (out);
}
