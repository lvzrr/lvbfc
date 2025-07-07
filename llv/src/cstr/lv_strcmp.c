#include "cstr.h"

int	lv_strcmp(const char *a, const char *b)
{
	size_t	c;

	if ((!a && b) || (a && !b))
		return (-1);
	if (!a && !b)
		return (0);
	c = 0;
	while (a[c] && b[c])
	{
		if (a[c] != b[c])
			return (a[c] - b[c]);
		c++;
	}
	return (a[c] - b[c]);
}
