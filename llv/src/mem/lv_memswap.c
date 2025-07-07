#include "mem.h"

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 256
#endif

lv_INLINE inline static t_u8	lv_memswap_dyn(void *__restrict__ p1,
	void *__restrict__ p2, size_t len)
{
	void	*buffer;

	if (!p1 || !p2 || !len)
		return (0);
	buffer = lv_alloc(len * sizeof(t_u8));
	if (!buffer)
		return (0);
	lv_memcpy(buffer, p1, len);
	lv_memcpy(p1, p2, len);
	lv_memcpy(p2, buffer, len);
	lv_free(&buffer);
	return (1);
}

lv_INLINE inline static t_u8	lv_memswap_bounded(void *__restrict__ p1,
	void *__restrict__ p2, size_t len)
{
	t_u8	buffer[BUFFER_SIZE];

	if (!p1 || !p2 || !len)
		return (0);
	lv_memcpy(buffer, p1, len);
	lv_memcpy(p1, p2, len);
	lv_memcpy(p2, buffer, len);
	return (1);
}

t_u8	lv_memswap(void *__restrict__ p1,
	void *__restrict__ p2, size_t len)
{
	if (len <= BUFFER_SIZE)
		return (lv_memswap_bounded(p1, p2, len));
	else
		return (lv_memswap_dyn(p1, p2, len));
}

lv_INLINE inline t_u8	lv_memswap_extern(void *__restrict__ p1,
	void *__restrict__ p2, size_t len,
	void *__restrict__ buffer)
{
	if (!p1 || !p2 || !len)
		return (0);
	lv_memcpy(buffer, p1, len);
	lv_memcpy(p1, p2, len);
	lv_memcpy(p2, buffer, len);
	return (1);
}
