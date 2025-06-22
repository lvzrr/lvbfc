#include "lvbfc.h"

static inline	void phelp(void)
{
	printf(
		"lvbfc - Brainfuck compiler\n"
		"---------------------------------------------------\n"
		"Usage:\n"
		"  ./lvbfc <input.b> [output] [options]\n"
		"\n"
		"Arguments:\n"
		"  <input.b>         Brainfuck source file (required)\n"
		"  [output]          Output binary name (optional)\n"
		"                    Default: bfout\n"
		"\n"
		"Options:\n"
		"  --no-strict       Disable safety checks for loops\n"
		"                    Ignores checks for potentially infinite loops\n"
		"\n"
		"  --no-wrap         Disable cell wraparound (executes faster)\n"
		"                    Allows 8-bit overflow behavior to be undefined\n"
		"\n"
		"  --opts            Use heap allocation for memory (dynamic growth)\n"
		"                    Enables GROW_BUF and dynamic pointer range\n"
		"\n"
		"  --stacksize=N     Set initial memory size in bytes\n"
		"                    Default: 65536 (64 KiB)\n"
		"\n"
		"  --opt-level=N     Set optimization level (0-LONG_MAX)\n"
		"                    Default: 3 (maximum optimizations)\n"
		"                    Note: bigger != better    \n"
		"\n"
		"  --dmp-tok         Print parsed token stream\n"
		"                    Useful for debugging and optimization testing\n"
		"\n"
		"  --help            Show this help message and exit\n"
		"\n"
		"Output:\n"
		"  Produces a compiled C file and builds it into a native binary.\n"
		"  Uses GCC or Clang automatically (fallback if one fails).\n"
		"  Output binary defaults to './bfout' unless specified.\n"
		"\n"
		"Examples:\n"
		"  ./lvbfc hello.b hello         # Compile hello.b to ./hello\n"
		"  ./lvbfc code.b --opts         # Use heap-allocated memory\n"
		"  ./lvbfc file.b --stacksize=0  # Will error out (invalid stacksize)\n"
		"\n"
		"Suggestions:\n"
		"  - Fastest + Safer:     --opts           (grows memory on demand + wraps in underflows)\n"
		"  - Mid:                 --no-wrap        (you control stacksize manually)\n"
		"  - Slowest:                              Default wrapping (safe but performs worst)\n"
		"\n"
	);
}

char	*read_file(const char *name)
{
	FILE *f = fopen(name,"rb");
	if (!f)
	{
		fprintf(stderr, "unable to read %s\n", name);
		return (NULL);
	}
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	rewind(f);
	char *src = malloc(fsize + 1);
	if (!src)
	{
		fprintf(stderr, "unable to allocate space for %s\n", name);
		return (NULL);
	}
	int c;
	size_t i = 0;
	while ((c = fgetc(f)) != EOF)
	{
		if (!(c == '>' || c == '<' || c == '+' || c == '-' ||
			c == '.' || c == ',' || c == '[' || c == ']'))
			continue ;
		src[i++] = c;
	}
	src[i] = 0;
	fclose(f);
	return (src);
}

t_vec	lex(const char *src, bool strict, bool dmp)
{
	char		curr = 0;
	char		*start = 0;
	t_tokenseq	x = {0};
	t_tokenseq	*prev = {0};
	t_vec		out = {0};
	const char 		*ogp = src;

	out = lv_vec(100, sizeof(t_tokenseq));
	while (*src)
	{
		curr = *src;
		start = (char *)src;
		while (*src && *src == curr)
			src++;
		x = (t_tokenseq) {.op = curr, .len = (size_t)(src - start)};
		if (out.size > 0)
			prev = lv_vec_peek_last(&out);
		if (prev && prev->op && prev->len)
		{
			if (x.op == ']' && prev->op == '[')
			{
				ssize_t	diff = x.len - prev->len;
				if (!diff)
				{
					lv_vec_pop(&out);
					continue ;
				}
				else if (diff > 0)
				{
					x.len -= prev->len;
					lv_vec_pop(&out);
				}
				else
				{
					prev->len -= x.len;
					continue ;
				}
			}
			else if (((x.op == '+' && prev->op == '-') || (x.op == '-' && prev->op == '+'))
				&& x.len == prev->len)
			{
				lv_vec_pop(&out);
				continue ;
			}
			else if ((x.op == '+' && prev->op == '-') || (x.op == '-' && prev->op == '+')
				|| (x.op == '>' && prev->op == '<') || (x.op == '<' && prev->op == '>'))
			{
				ssize_t	diff = prev->len - x.len;
				if (diff > 0)
					prev->len = prev->len - x.len;
				else
				{
					size_t	pl = prev->len;
					*prev = x;
					prev->len = x.len - pl;
				}
				continue ;
			}
			if (x.op == '[' && *(src) == '-' && *(src + 1) == ']')
			{
				x.len--;
				if (!x.len)
				{
					x.op = 'Z';
					x.len = 1;
					src += 2;
				}
				else
				{
					lv_vec_push(&out, &x, 1);
					x.op = 'Z';
					x.len = 1;
					src += 2;
				}
			}
		}
		lv_vec_push(&out, &x, 1);
	}
	size_t plevel = 0;
	for (size_t i = 0; i < out.size; i++)
	{
		t_tokenseq *x2 = ((t_tokenseq *)lv_vec_get(&out, i));
		if (x2->op == '[')
			plevel += x2->len;
	  	else if (x2->op == ']')
			plevel -= x2->len;
		if (x2->op == '[' && i + 2 < out.size
			&& (x2 + 1)->op != '-' && (x2 + 1)->len == 1 && (x2 + 2)->op == ']')
			{
				if (strict)
				{
					free((char *)ogp);
					THROW_ERR_AT(i, "potential infinite loop [%c] detected in the code (run with --no-strict to ignore)", out, (x2 + 1)->op);
				}
				else
					fprintf(stderr, "\033[1;33mWARNING: potential infinite loop [%c] detected @ seq. %lu\033[0m\n", (x2 + 1)->op, i);
			}
		if (x2->op == '[')
		{
			size_t i2 = i + 1;
			bool hasm = false;
			while (i2 < out.size)
			{
				const t_tokenseq *y = lv_vec_get(&out, i2);
				if (y->op == ']')
					break;
				if (y->op == '-')
				{
					hasm = true;
					break;
				}
				i2++;
			}
			if (strict && !hasm)
			{
				free((char *)ogp);
				THROW_ERR_AT(i, "potential infinite loop (no '-') detected (run with --no-strict to ignore)", out);
			}
			else if (!strict && !hasm)
				fprintf(stderr, "\033[1;33mWARNING: potential dangerous loop (no '-' in loop) detected @ seq. %lu\033[0m\n", i);
		}

		if (dmp)
			printf("op: %c  len: %lu plevel: %ld\n", x2->op, x2->len, plevel);
	}
	if (plevel != 0)
	{
		free((char *)ogp);
		THROW_ERR("unmatched loop guards", out);
	}
	return (free((char *)ogp), out);
}

void compile_c(char *name)
{
	char cmd[256] = {0};
	bool ok = false;
	snprintf(cmd, sizeof(cmd),
		"cc -Wall -Wextra -Werror -O3 -march=native -msse -msse2 -mavx -mavx2 "SEP"tmp"SEP"bf.c -o %s", name);
	if (system(cmd) == 0)
		ok = true;
	else
	{
		snprintf(cmd, sizeof(cmd),
			"clang -Wall -Wextra -Werror -O3 -march=native -msse -msse2 -mavx -mavx2 "SEP"tmp"SEP"bf.c -o %s", name);
		if (system(cmd) == 0)
			ok = true;
	}
	if (!ok)
	{
		fprintf(stderr, "\033[1;31mNo working C compiler detected.\033[0m\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	char	*filename = NULL;
	char	*outname = "bfout";
	bool	strict = true;
	bool	shstrm = false;
	bool	wrap = true;
	bool	heap = false;
	size_t	stsize = 65536;
	size_t	optl = 3;

	if (argc < 2)
	{
		phelp();
		exit(EXIT_FAILURE);
	}
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--no-strict") == 0) 
			strict = false;
		else if (strcmp(argv[i], "--dmp-tok") == 0) 
			shstrm = true;
	  	else if (strcmp(argv[i], "--no-wrap") == 0) 
			heap = true;
	  	else if (strcmp(argv[i], "--opts") == 0) 
			wrap = false;
		else if (strncmp(argv[i], "--stacksize=", 12) == 0)
		{
			stsize = lv_atoul(*(argv + i) + 12);
			if (!stsize)
			{
				fprintf(stderr, "Invalid stacksize\n");
				return (EXIT_FAILURE);
			}
		}
		else if (strncmp(argv[i], "--opt-level=", 12) == 0)
			optl = lv_atoul(*(argv + i) + 12);
		else if (strcmp(argv[i], "--help") == 0) 
		{
			phelp();
			return (0);
		}
		else if (strncmp(argv[i], "--", 2) == 0) 
		{
			fprintf(stderr, "Unknown flag: %s\n\n", argv[i]);
			phelp();
			return (EXIT_FAILURE);
		}
		else if (!filename) 
			filename = argv[i];
		else
			outname = argv[i];
	}
	if (!filename)
	{
		fprintf(stderr, "no file to compile\n");
		return EXIT_FAILURE;
	}
	char *src = read_file(filename);
	if (!src)
		return EXIT_FAILURE;
	t_vec o = lex(src, strict, shstrm);
	if (shstrm)
	{
		optimize(&o, optl);
		printf("==OPT==\n");
		for (size_t i = 0; i < o.size; i++) {
			const t_tokenseq *t = lv_vec_get(&o, i);
			if (t)
				printf("op: %c, len: %zu\n", t->op, t->len);
		}
		lv_vec_free(&o);
		return (0);
	}
	printf("[lvbfc] compiling\n");
	if (heap)
		emit_heap(&o, optl);
	else
		emit(&o, wrap, stsize);
	compile_c(outname);
	printf("[lvbfc] compililed successfully!\n");
	return (0);
}
