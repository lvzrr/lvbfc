#ifndef CSTR_H
# define CSTR_H
# include <unistd.h>
# include <stdlib.h>
# include <sys/types.h>
# include <stdint.h>
# include "structs.h"
# include "alloc.h"
# include "mem.h"

int				lv_tolower(int c);
int				lv_toupper(int c);
size_t			lv_strlen(const char *str);
char			*lv_strdup(const char *str);
char			*lv_strchr(const char *haystack, int needle);
char			*lv_strchr_small(const char *haystack, int needle);
char			*lv_strrchr(const char *haystack, int needle);
size_t			lv_strlcpy(char *dest, const char *src, size_t n);
size_t			lv_strlcat(char *dest, const char *src, size_t n);
int				lv_strncmp(const char *s1, const char *s2, size_t n);
char			*lv_strnstr(const char *haystack, const char *needle,
					size_t n);
char			*lv_strtrim(const char *str, const char *set);
char			*lv_strmapi(char const *str, char (*f)(unsigned int, char));
void			lv_striteri(char *str, void (*f)(unsigned int, char *));
char			*lv_strjoin(const char *s1, const char *s2);
char			*lv_substr(const char *s, unsigned int start, size_t len);
char			**lv_split(const char *str, char set);
int				lv_strcmp(const char *a, const char *b);
t_u8			lv_isnumeric(const char *__restrict__ s);
#endif
