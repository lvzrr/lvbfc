#include "cstr.h"

static int	in_set(char c, const char *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (i);
		i++;
	}
	return (-1);
}

char	*lv_strtrim(const char *str, const char *set)
{
	size_t	b;
	size_t	e;
	size_t	trimmed_len;
	char	*out;

	if (!str)
		return (NULL);
	if (!set || !set[0] || !str[0])
		return (lv_strdup(str));
	b = 0;
	e = lv_strlen(str) - 1;
	while (str[b] && in_set(str[b], set) >= 0)
		b++;
	while (e >= b && in_set(str[e], set) >= 0)
		e--;
	if (b > e)
		trimmed_len = 0;
	else
		trimmed_len = e - b + 1;
	out = lv_alloc(trimmed_len + 1);
	if (!out)
		return (NULL);
	lv_memcpy(out, str + b, trimmed_len);
	out[trimmed_len] = '\0';
	return (out);
}
