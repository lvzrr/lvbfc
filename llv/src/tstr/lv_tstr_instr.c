#include "cstr.h"
#include "tstr.h"

static ssize_t	lv_find(const char *haystack, const char *needle, size_t n,
							size_t l)
{
	size_t	i;

	i = 0;
	l = 0;
	if (!*needle)
		return (0);
	if (!*haystack)
		return (-1);
	while (haystack[i] && (i + l) <= n)
	{
		if (haystack[i] == needle[0] && !lv_strncmp(&(haystack[i]), needle, l))
			return ((ssize_t) i);
		i++;
	}
	return (-1);
}

ssize_t	lv_tstr_instr(const t_string *h, const char *n)
{
	size_t	l;

	if (!h ||!h->data || !n)
		return (-1);
	l = lv_strlen(n);
	return (lv_find(lv_tstr_borrow(h), n, h->len, l));
}
