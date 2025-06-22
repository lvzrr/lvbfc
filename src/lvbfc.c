#include "lvbfc.h"

static inline void phelp(void)
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
		"  --no-strict       Disable safety checks for loops @ comptime\n"
		"\n"
		"  --no-wrap         Disable buffer wraparound (executes faster)\n"
		"\n"
		"  --opts            Use heap allocation for dynamic memory growth\n"
		"\n"
		"  --x               Enable syscall shellcode emit mode (experimental)\n"
		"                    Use `;` x N to mark syscall byte count\n"
		"\n"
		"  --stacksize=N     Initial memory size in bytes (default: 65536)\n"
		"\n"
		"  --opt-level=N     Optimization passes (default: 3)\n"
		"                    More ≠ better (balance iteration vs comptime)\n"
		"\n"
		"  --dmp-tok         Print parsed token stream (debugging only)\n"
		"\n"
		"  --help            Show this help message and exit\n"
		"\n"
		"Output:\n"
		"  Produces a .c + native binary via GCC/Clang (fallback if needed).\n"
		"  Output binary defaults to './bfout' unless specified.\n"
		"\n"
		"Examples:\n"
		"  ./lvbfc hello.b hello         # compile to ./hello\n"
		"  ./lvbfc code.b --opts         # safer & dynamic memory growth\n"
		"  ./lvbfc syscall.b --x         # run syscall via shellcode\n"
		"  ./lvbfc file.b --stacksize=0  # invalid → error\n"
		"\n"
		"Suggestions:\n"
		"  - Fastest:             --no-wrap --opt-level=5\n"
		"  - Mid‑range:           --opts --opt-level=5 (heap)\n"
		"  - Shellcode mode:      --x with `;;;;;;` to exec 6‑byte syscall (the same as --opts)\n"
		"  - Safe default:        no flags\n"
	);
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
	size_t	optl = 1;
	bool	x = false;

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
			wrap = false;
	  	else if (strcmp(argv[i], "--opts") == 0) 
			heap = true;
	  	else if (strcmp(argv[i], "--x") == 0) 
			x = true;
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
		{
			optl = lv_atoul(*(argv + i) + 12);
			optl = (optl > 0) ? 1 : 0;
		}
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
	char *src = read_file(filename, x);
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
		emit_heap(&o, optl, x);
	else
		emit(&o, wrap, stsize, optl, x);
	compile_c(outname);
	printf("[lvbfc] compililed successfully!\n");
	return (0);
}
