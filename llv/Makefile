NAME		:=	liblv.a
CC			:=	cc
FLAGS		:=	-Wall -Wextra -Werror -O3 -Wpedantic
AR			:=	ar rcs
OBJDIR		:=	build
DIRS		:=	math map vec tstr cstr alloc ctype mem conv io lst mem/mem_helpers

SRCS		:=	$(foreach dir, $(DIRS), $(wildcard src/$(dir)/*.c))
OBJS		:=	$(patsubst %.c, $(OBJDIR)/%.o, $(SRCS))

all: $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@ -Iinclude

$(NAME): $(OBJS)
	@$(AR) $@ $^

install: all
	@sudo cp $(NAME) /usr/local/lib
	@sudo mkdir -p /usr/local/include/llv/
	@sudo cp include/* /usr/local/include/llv/

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

test: install test-mem test-cstr

re: fclean full all

.PHONY: all clean fclean re bonus install full
MAKEFLAGS += --no-print-directory
