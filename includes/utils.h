#ifndef UTILS_H
# define UTILS_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_path_search
{
	char	*path;
	char	*tok;
	char	*temp;
	char	*res;
}	t_path_search;

typedef struct s_check_cmd
{
	char	*cmd_dir;
	int		is_success;
}	t_check_cmd;

char		*strjoin(char *s1, char *s2);
t_check_cmd	check_cmd(char *cmd);

#endif