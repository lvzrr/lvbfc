CC      = cc
CFLAGS  = -Wall -Wextra -Werror -O3 -I./include -I./llv/include -march=native -msse -msse2 -mavx -mavx2
LIBLV   = llv/liblv.a
SRC     = src/lvbfc.c src/emmiters.c src/optm.c src/lexer.c
OUT     = lvbfc
LLV_REPO = https://github.com/lvzrr/llv

.PHONY: all clean fclean re

all: $(OUT)

$(OUT): $(SRC) $(LIBLV)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBLV):
	@if [ ! -d llv ]; then \
		echo "Cloning llv..."; \
		git clone $(LLV_REPO) llv; \
	fi
	@echo "Building liblv..."
	$(MAKE) -C llv

clean:
	@if [ -d llv ]; then $(MAKE) -C llv clean; fi
	@rm -f $(OUT)

fclean: clean
	@if [ -d llv ]; then $(MAKE) -C llv fclean; fi
	@rm -f $(OUT)

re: fclean all
