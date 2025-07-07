#include "io.h"

static inline void	readbuf(t_string *store, int fd, size_t *off)
{
	t_u8		buffer[BUFSIZE];
	ssize_t		res;

	res = read(fd, buffer, BUFSIZE);
	if (res < 0)
	{
		lv_tstr_free(store);
		*off = 0;
		return ;
	}
	lv_tstr_pushslice(store, (const char *)buffer, res);
}

int	lv_fgetc(int fd)
{
	static t_string	store = {0};
	static size_t	offset = {0};

	if (fd < 0)
		return (EOF);
	if (!store.alloc_size)
	{
		store = lv_tstr_new(BUFSIZE);
		if (!store.data)
			return (EOF);
	}
	if (store.len == offset)
	{
		offset = 0;
		lv_tstr_clear(&store);
		readbuf(&store, fd, &offset);
	}
	if (store.len)
		return ((unsigned char)store.data[offset++]);
	else
		return (lv_tstr_free(&store), EOF);
}
