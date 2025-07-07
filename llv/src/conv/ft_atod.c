#include "conv.h"

static inline double	__eat_decimal(const char *s)
{
	double	c;
	double	out;

	c = 0.1f;
	out = 0;
	while (*s && (*s >= '0' && *s <= '9'))
	{
		out += c * (*s++ - '0');
		c *= 0.1f;
	}
	return (out);
}

double	lv_atod(const char *s)
{
	int		neg;
	double	out;

	if (!s)
		return (0);
	out = 0;
	neg = 1;
	if (*s == '-')
	{
		neg = -1;
		s++;
	}
	else if (*s == '+')
		s++;
	while (*s && *s != '.' && (*s >= '0' && *s <= '9'))
		out = (out * 10) + (*s++ - '0');
	if (*s == '.')
		s++;
	out += __eat_decimal(s);
	return (out * neg);
}
