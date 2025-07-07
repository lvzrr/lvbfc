#include "llv.h"

static unsigned int	_abs(int n)
{
	if (n < 0)
		return ((unsigned int)-n);
	return ((unsigned int)n);
}

static size_t	count(int n, int blen)
{
	size_t	c;

	c = 0;
	if (n < 0)
		c++;
	while (n != 0)
	{
		c++;
		n /= blen;
	}
	return (c);
}

static int	lv_lvalidate_base(char *base)
{
	char	*ptr;
	int		l;

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

char	*lv_itoa_base(int n, char *base)
{
	int			blen;
	char		*out;
	size_t		c;

	blen = lv_lvalidate_base(base);
	if (blen == -1)
		return (NULL);
	if (n == 0)
		return (lv_strdup("0"));
	out = lv_alloc(count(n, blen) + 1);
	if (!out)
		return (NULL);
	c = count(n, blen);
	out[c] = 0;
	while (n != 0)
	{
		out[--c] = base[_abs(n % blen)];
		n /= blen;
	}
	if (c == 1)
		out[--c] = '-';
	return (out);
}
