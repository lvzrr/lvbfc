#include "io.h"

t_string	get_next_line(int fd)
{
	char			c;
	static t_string	out;
	t_string		tmp;

	if (fd < 0)
		return ((t_string){0});
	if (!out.alloc_size)
	{
		out = lv_tstr_new(BUFSIZE);
		if (!out.data)
			return ((t_string){0});
	}
	c = lv_fgetc(fd);
	while (c != EOF && c != '\n')
	{
		lv_tstr_push(&out, c);
		c = lv_fgetc(fd);
	}
	if (c == '\n')
		lv_tstr_push(&out, c);
	if (c == EOF)
		return (lv_tstr_free(&out), (t_string){0});
	tmp = lv_tstr_clone(&out);
	return (lv_tstr_clear(&out), tmp);
}
