#ifndef CONV_H
# define CONV_H
# include <unistd.h>
# include <stdlib.h>

int				lv_atoi(const char *str);
ssize_t			lv_atol(const char *str);
float			lv_atof(const char *s);
double			lv_atod(const char *s);
int				lv_atoi_base(char *str, char *base, int base_len);
char			*lv_itoa(int n);
char			*lv_itoa_base(int n, char *base);
char			*lv_utoa_base(size_t n, char *base);
size_t			lv_atoul(const char *str);
#endif
