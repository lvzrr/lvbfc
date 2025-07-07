#include "llv.h"

static void	*errorhandle(t_list *newl, void *tmp, void (*del)(void *))
{
	del(tmp);
	lv_lstclear(&newl, del);
	return (NULL);
}

t_list	*lv_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	void	*tmp;
	t_list	*newl;
	t_list	*newnode;

	if (!lst || !f || !del)
		return (NULL);
	newl = NULL;
	while (lst)
	{
		tmp = f(lst->content);
		newnode = lv_lstnew(tmp);
		if (!newnode)
			return (errorhandle(newl, tmp, del));
		lv_lstadd_back(&newl, newnode);
		lst = lst->next;
	}
	return (newl);
}
