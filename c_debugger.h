#define _POSIX_C_SOURCE 200809L
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

typedef struct s_path_search
{
	char	*path;
	char	*tok;
	char	*temp;
	char	*res;
}	t_path_search;