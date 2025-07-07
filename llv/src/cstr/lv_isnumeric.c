#include "llv.h"

inline t_u8	lv_isnumeric(const char *__restrict__ s)
{
	t_u8	has_digit;

	if (!s || !*s)
		return (0);
	has_digit = 0;
	while (*s)
	{
		if (*s >= '0' && *s <= '9')
			has_digit = 1;
		else if (*s != '-' && *s != '+' && *s != ' ')
			return (0);
		s++;
	}
	return (has_digit);
}
