#ifndef PUT_H
# define PUT_H
# include <unistd.h>

void			lv_putchar_fd(char c, int fd);
void			lv_putstr_fd(const char *str, int fd);
void			lv_putendl_fd(const char *str, int fd);
void			lv_putnbr_fd(int n, int fd);
#endif
