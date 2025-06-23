#ifndef BFT_H
# define  BFT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llv/vec.h"
#include "llv/mem.h"
#include "llv/conv.h"

#define THROW_ERR(msg, vec, ...) \
	do { \
		fprintf(stderr, "\033[1;31m" msg "\033[0m\n", ##__VA_ARGS__); \
		lv_vec_free(&vec); \
		exit(EXIT_FAILURE); \
	} while (0)

#define THROW_ERR_AT(i, msg, vec, ...) \
	do { \
		fprintf(stderr, "\033[1;31merror in %lu: " msg "\033[0m\n", i, ##__VA_ARGS__); \
		lv_vec_free(&vec); \
		exit(EXIT_FAILURE); \
	} while (0)

#ifdef __WIN32
# define SEP "\\"
# else
# define SEP "/"
#endif


typedef	struct tokenseq
{
	char 	op;
	size_t	len;
}	t_tokenseq;

void	emit(t_vec *v, bool w, size_t stsize, size_t l, bool x);
void	emit_heap(t_vec *v, size_t stsize, size_t l, bool x);
void	optimize(t_vec *v, size_t l);
t_vec	lex(const char *src, bool strict, bool dmp);
char	*read_file(const char *name, bool x, bool can, bool intr);
#endif
