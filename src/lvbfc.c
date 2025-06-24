#include "lvbfc.h"

static inline void	phelp(void)
{
	printf(
		"lvbfc - Brainfuck compiler\n"
		"---------------------------------------------------\n"
		"Usage:\n"
		"  ./lvbfc <input.b> [output] [options]\n"
		"\n"
		"Arguments:\n"
		"  <input.b>             Brainfuck source file (required)\n"
		"  [output]              Output binary name (optional)\n"
		"                        Default: bfout\n"
		"\n"
		"Options:\n"
		"  --no-strict           Disable compile-time loop validation\n"
		"  --no-wrap             Disable tape wraparound (exits on overflow)\n"
		"  --heap                Use dynamic memory instead of fixed-size tape\n"
		"  --turbo               Skip all bounds checks (manual pointer safety)\n"
		"  --x                   Enable syscall emit mode using ';' bytes\n"
		"  --allow-canary        Enable '?' and '??' debug markers + memory dump\n"
		"  --allow-intrinsics    Enable '$' for memset, math, and other builtins\n"
		"  --stacksize=N         Initial tape size (default: 65536 bytes)\n"
		"  --opt-level=N         Optimizer passes (0 = off, >0 = enabled)\n"
		"  --dmp-tok             Dump token stream before compilation (debug)\n"
		"  --help                Show this help message and exit\n"
		"\n"
		"Output:\n"
		"  Produces .c and native binary using GCC or Clang (fallback supported)\n"
		"\n"
		"Examples:\n"
		"  ./lvbfc hello.b hello           # compile to ./hello\n"
		"  ./lvbfc code.b --heap           # dynamic memory mode\n"
		"  ./lvbfc syscall.b --x           # emit + exec syscall shellcode\n"
		"  ./lvbfc file.b --stacksize=0    # compile error: invalid size\n"
		"\n"
		"Suggestions:\n"
		"  - Fastest:             --no-wrap --opt-level=1 --turbo\n"
		"  - Mid‑range:           --heap\n"
		"  - Shellcode mode:      --x with ';;;;;;' to emit 6‑byte syscall\n"
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
	size_t	optl = 0;
	bool	x = false;
	bool	can = false;
	bool	intr = false;
	bool	turbo = false;

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
	  	else if (strcmp(argv[i], "--allow-canary") == 0) 
			can = true;
	  	else if (strcmp(argv[i], "--allow-intrinsics") == 0) 
			intr = true;
	  	else if (strcmp(argv[i], "--heap") == 0) 
			heap = true;
	  	else if (strcmp(argv[i], "--turbo") == 0) 
			turbo = true;
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
		else if (*argv[i] == '-') 
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
	char *src = read_file(filename, x, can, intr);
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
		emit_heap(&o, stsize, optl, x);
	else
		emit(&o, wrap, stsize, optl, x, turbo);
	compile_c(outname);
	printf("[lvbfc] compiled successfully!\n");
	return (0);
}
