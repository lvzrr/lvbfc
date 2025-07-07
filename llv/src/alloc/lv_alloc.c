#include "alloc.h"

__attribute__((malloc))
void	*lv_alloc_align(size_t size, size_t align)
{
	void	*tab;
	size_t	alloc_size;
	t_uptr	cc;
	void	*ac;

	if ((align == 0) || ((align & (align - 1)) != 0))
		return (NULL);
	if (align < sizeof(void *))
		align = sizeof(void *);
	if (size > SIZE_MAX - (align - 1) - sizeof(void *))
		return (NULL);
	alloc_size = sizeof(void *) + (align - 1) + size;
	tab = malloc(alloc_size);
	if (!tab)
		return (NULL);
	cc = (t_uptr)tab + sizeof(void *);
	ac = (void *)((cc + (align - 1)) & ~(align - 1));
	return (((void **)ac)[-1] = tab, ac);
}

__attribute__((malloc))
void	*lv_alloc(size_t size)
{
	void	*new_alloc;

	new_alloc = lv_alloc_align(size, DEF_ALIGN);
	if (!new_alloc)
		return (NULL);
	return (new_alloc);
}
