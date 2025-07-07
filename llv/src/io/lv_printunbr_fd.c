#include "llv.h"

void	lv_printunbr_fd(unsigned int n, int fd, int *len)
{
	char	o;

	if (n >= 10)
	{
		lv_printnbr_fd(n / 10, fd, len);
		lv_printnbr_fd(n % 10, fd, len);
	}
	else
	{
		o = (n % 10) + '0';
		*len = *len + 1;
		write(fd, &o, 1);
	}
}
