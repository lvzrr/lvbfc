#ifndef IO_H
# define IO_H
# include <stdarg.h>
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/types.h>
# include "cstr.h"
# include "tstr.h"
# include "mem.h"
# include "alloc.h"

# ifndef EOF
#  define EOF -1
# endif

# ifndef BUFSIZE
#  define BUFSIZE 4096
# endif

t_string		get_next_line(int fd);
int				lv_printchar_fd(const char c, int fd);
int				lv_printstr_fd(const char *s, int fd);
void			lv_printnbr_fd(int n, int fd, int *c);
void			lv_printunbr_fd(unsigned int n, int fd, int *c);
void			lv_printptr_fd(unsigned long p, int *c, int fd);
int				lv_printf(const char *fmt, ...);
void			lv_printhex_fd(unsigned int x, const char *set, int *l, int fd);
int				lv_fprintf(int fd, const char *fmt, ...);
int				lv_fgetc(int fd);
#endif
