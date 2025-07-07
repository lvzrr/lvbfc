#include "llv.h"

static int	is_flag(char c)
{
	return (c == 'c' || c == 'd' || c == 'u'
		|| c == 'p' || c == 'x'
		|| c == 'X' || c == 'u'
		|| c == 's' || c == 'i'
		|| c == '%');
}

static int	numhelper(int fd, char c, va_list args)
{
	int	l;

	l = 0;
	if (c == 'u')
		lv_printunbr_fd(va_arg(args, unsigned int), fd, &l);
	else if (c == 'x')
		lv_printhex_fd(va_arg(args, unsigned int),
			"0123456789abcdef", &l, fd);
	else if (c == 'X')
		lv_printhex_fd(va_arg(args, unsigned int),
			"0123456789ABCDEF", &l, fd);
	else
		lv_printnbr_fd(va_arg(args, int), fd, &l);
	return (l);
}

static int	ptrhelper(int fd, va_list args)
{
	int				l;
	unsigned long	p;

	p = (unsigned long)va_arg(args, void *);
	l = 0;
	lv_printptr_fd(p, &l, fd);
	return (l);
}

static int	dispatch(int fd, char fmt, va_list args)
{
	if (fmt == 'c')
		return (lv_printchar_fd((char)va_arg(args, int), fd));
	else if (fmt == '%')
		return (lv_printchar_fd('%', fd));
	else if (fmt == 's')
		return (lv_printstr_fd(va_arg(args, char *), fd));
	else if (fmt == 'i' || fmt == 'd')
		return (numhelper(fd, fmt, args));
	else if (fmt == 'u')
		return (numhelper(fd, fmt, args));
	else if (fmt == 'p')
		return (ptrhelper(fd, args));
	else if (fmt == 'x')
		return (numhelper(fd, fmt, args));
	else if (fmt == 'X')
		return (numhelper(fd, fmt, args));
	return (0);
}

int	lv_fprintf(int fd, const char *fmt, ...)
{
	va_list		args;
	int			c;

	c = 0;
	va_start(args, fmt);
	if (!fmt || !*fmt)
		return (0);
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			if (*fmt && is_flag(*fmt))
				c += dispatch(fd, *fmt, args);
			fmt++;
			continue ;
		}
		write (fd, fmt, 1);
		c++;
		fmt++;
	}
	va_end(args);
	return (c);
}
