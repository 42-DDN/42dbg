#ifndef DBG_H
# define DBG_H

#include <utils.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <readline/readline.h>

typedef struct s_dbg
{
	char	*cmd;
	char 	*cmd_dir;
}	t_dbg;


#endif