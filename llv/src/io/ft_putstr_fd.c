#include "llv.h"

void	lv_putstr_fd(const char *str, int fd)
{
	size_t	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
		write(fd, &(str[i++]), 1);
}
