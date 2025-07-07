#include "alloc.h"

static t_arena	*lv_init_arena(size_t size)
{
	size_t	total_size;
	t_arena	*arena;

	arena = lv_alloc(sizeof(t_arena));
	if (!arena)
		return (NULL);
	total_size = __max_s(size * 2, DEFAULT_ARENA_SIZE);
	arena->pool = lv_alloc(total_size);
	if (!arena->pool)
		return (NULL);
	arena->size = total_size;
	arena->offset = 0;
	arena->next = NULL;
	return (arena);
}

static void	*lv_manage_req(t_arena *arena, size_t size)
{
	void	*base;
	void	*aligned;
	size_t	padding;

	base = (t_u8 *)arena->pool + arena->offset + sizeof(void *);
	aligned = (void *)(((t_uptr)base + (DEF_ALIGN - 1)) & ~(DEF_ALIGN - 1));
	padding = (t_u8 *)aligned - (t_u8 *)base;
	if (arena->offset + padding + size > arena->size)
	{
		if (!arena->next)
			arena->next = lv_init_arena(size);
		if (!arena->next)
			return (NULL);
		return (lv_manage_req(arena->next, size));
	}
	arena->offset += padding + size;
	return (aligned);
}

void	*lv_arena(size_t size)
{
	static t_arena	*arena;
	static t_arena	*current;

	if (!size)
		return (NULL);
	if (!arena)
	{
		arena = lv_init_arena(size);
		lv_free_arena(arena);
		current = arena;
	}
	if (!arena)
		return (NULL);
	while (current->next)
		current = current->next;
	return (lv_manage_req(current, size));
}

void	lv_free_arena(t_arena *arena)
{
	static t_arena	*store;
	t_arena			*current_arena;
	t_arena			*next_arena;
	void			*t;

	if (arena)
	{
		store = arena;
		atexit((void (*)(void))lv_free_arena);
		return ;
	}
	if (!store)
		return ;
	current_arena = store;
	while (current_arena)
	{
		next_arena = current_arena->next;
		t = current_arena->pool;
		if (current_arena->pool)
			lv_free(&t);
		t = current_arena;
		lv_free(&t);
		current_arena = next_arena;
	}
	store = NULL;
}
