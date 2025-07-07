#include "llv.h"

void	lv_putendl_fd(const char *s, int fd)
{
	lv_putstr_fd(s, fd);
	write(fd, "\n", 1);
}
