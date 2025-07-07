#include "tstr.h"

static int	in_set(const char c, const char *set)
{
	size_t	i;

	i = 0;
	while (set[i])
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

void	lv_tstr_trim(t_string *str, const char *set)
{
	size_t	start;
	size_t	end;
	size_t	new_len;

	if (!str || !str->data || !str->len || !set)
		return ;
	start = 0;
	end = str->len;
	while (start < end && in_set(str->data[start], set))
		start++;
	while (end > start && in_set(str->data[end - 1], set))
		end--;
	new_len = end - start;
	lv_memmove(str->data, str->data + start, new_len);
	if (str->alloc_size > new_len)
		lv_memset(str->data + new_len, 0, str->alloc_size - new_len);
	str->len = new_len;
}
