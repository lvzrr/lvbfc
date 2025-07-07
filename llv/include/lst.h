#ifndef LST_H
# define LST_H
# include <sys/types.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;

t_list			*lv_lstnew(void *contents);
int				lv_lstsize(t_list *lst);
void			lv_lstadd_front(t_list **lst, t_list *n);
t_list			*lv_lstlast(t_list *lst);
void			lv_lstadd_back(t_list **lst, t_list *n);
void			lv_lstdelone(t_list *lst, void (*del)(void *));
void			lv_lstclear(t_list **lst, void (*del)(void *));
void			lv_lstiter(t_list *lst, void (*f)(void *));
t_list			*lv_lstmap(t_list *lst, void *(*f)(void *),
					void (*del)(void *));
#endif
