#include "vec.h"

t_vec	lv_vec(size_t alloc_size, size_t sizeof_type)
{
	t_vec	out;

	if (!alloc_size || !sizeof_type)
		return ((t_vec){0});
	if (SIZE_MAX / alloc_size <= sizeof_type)
		return ((t_vec){0});
	out.size = 0;
	out.alloc_size = alloc_size;
	out.data = lv_alloc(alloc_size * sizeof_type);
	if (!out.data)
		return ((t_vec){0});
	out.sizeof_type = sizeof_type;
	return (out);
}
