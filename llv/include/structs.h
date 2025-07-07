#ifndef STRUCTS_H
# define STRUCTS_H

# include <sys/types.h>
# include <stdint.h>
# include "macros.h"

typedef unsigned char __attribute__((__may_alias__))	t_u8;
typedef uint32_t __attribute__((__may_alias__))			t_u32;
typedef uint64_t __attribute__((__may_alias__))			t_u64;
typedef __uint128_t __attribute__((__may_alias__))		t_u128;
typedef uintptr_t __attribute__((__may_alias__))		t_uptr;

typedef struct s_string
{
	size_t	len;
	size_t	alloc_size;
	char	*data;
}	t_string;

typedef struct s_vec
{
	size_t	size;
	void	*data;
	size_t	alloc_size;
	size_t	sizeof_type;
}	t_vec;

typedef struct s_arena
{
	size_t			size;
	size_t			offset;
	struct s_arena	*next;
	void			*pool;
}	t_arena;

#endif
