#include "llv.h"

int	lv_printstr_fd(const char *str, int fd)
{
	unsigned int	l;

	if (!str)
	{
		return (write(fd, "(null)", 6));
	}
	l = 0;
	while (str[l])
		l += write (fd, str + l, 1);
	return ((int) l);
}
