#include "llv.h"

static int	lv_check_in_base(char c, char *base)
{
	int	pos;

	pos = 0;
	while (*base)
	{
		if (*base++ == c)
			return (pos);
		pos++;
	}
	return (-1);
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

int	lv_atoi_base(char *str, char *base, int base_len)
{
	int				out;
	int				neg;

	if (base_len == -1 || lv_lvalidate_base(base) == -1)
		return (0);
	out = 0;
	neg = 1;
	if (!*str)
		return (0);
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	while (*str == '-' || *str == '+')
	{
		if (*str == '-')
			neg *= -1;
		str++;
	}
	while (lv_check_in_base(*str, base) >= 0)
		out = out * base_len + lv_check_in_base(*str++, base);
	return (out * neg);
}
