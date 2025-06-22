CC      = cc
CFLAGS  = -Wall -Wextra -Werror -O3 -I./include -I./llv/include -march=native -msse -msse2 -mavx -mavx2
LIBLV   = llv/liblv.a
SRC     = src/lvbfc.c src/emmiters.c src/optm.c src/lexer.c
OUT     = lvbfc

.PHONY: all clean fclean re

all: $(OUT)

$(OUT): $(SRC) $(LIBLV)
	$(CC) $(CFLAGS) -o $@ $^

$(LIBLV):
	@echo "Building liblv..."
	$(MAKE) -C llv

clean:
	$(MAKE) -C llv clean
	@rm -f $(OUT)

fclean: clean
	$(MAKE) -C llv fclean
	@rm -f $(OUT)

re: fclean all
