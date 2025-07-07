#include "llv.h"

void	lv_putnbr_fd(int n, int fd)
{
	char	o;

	if (n == -2147483648)
	{
		write(fd, "-2147483648", 11);
		return ;
	}
	if (n < 0)
	{
		n = -n;
		write(fd, "-", 1);
	}
	if (n >= 10)
	{
		lv_putnbr_fd(n / 10, fd);
		lv_putnbr_fd(n % 10, fd);
	}
	else
	{
		o = (n % 10) + '0';
		write(fd, &o, 1);
	}
}
