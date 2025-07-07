#include "llv.h"

static size_t	count(size_t n, size_t blen)
{
	size_t	c;

	c = 0;
	while (n != 0)
	{
		c++;
		n /= blen;
	}
	return (c);
}

static ssize_t	lv_lvalidate_base(char *base)
{
	char	*ptr;
	ssize_t	l;

	l = lv_strlen(base);
	ptr = base;
	if (l < 2)
		return (-1);
	while (*base)
	{
		ptr = base + 1;
		while (*ptr)
			if (*base == *ptr++
				|| *base == '-'
				|| *base == '+')
				return (-1);
		base++;
	}
	return (l);
}

char	*lv_utoa_base(size_t n, char *base)
{
	size_t		blen;
	ssize_t		blen1;
	char		*out;
	size_t		c;

	blen1 = lv_lvalidate_base(base);
	if (blen1 == -1)
		return (NULL);
	blen = (size_t) blen1;
	if (n == 0)
		return (lv_strdup("0"));
	out = lv_alloc(count(n, blen) + 1);
	if (!out)
		return (NULL);
	c = count(n, blen);
	out[c] = 0;
	while (n != 0)
	{
		out[--c] = base[n % blen];
		n /= blen;
	}
	if (c == 1)
		out[--c] = '-';
	return (out);
}
