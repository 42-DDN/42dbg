#include "c_debugger.h"

int	regs_dump(pid_t pid)
{
	struct user_regs_struct regs;
	if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)
	{
		perror("ptrace error");
		return (EXIT_FAILURE);
	}
	printf("Simple registeries dump:\n");
	printf("RAX: %llx\nRBX: %llx\nRCX: %llx\nRDX: %llx\nRSI: %llx\nRDI: %llx\nRIP: %llx\n\e[0m",
		regs.rax, regs.rbx, regs.rcx, regs.rdx, regs.rsi, regs.rdi, regs.rip);
	return (EXIT_SUCCESS);
}

int	wait_child(pid_t pid)
{
	int	wstatus;
	waitpid(pid, &wstatus, 0);
	if (WIFSIGNALED(wstatus))
	{
		printf("Child Terminated By the following signal: %d\n", WTERMSIG(wstatus));
		return (0);
	}
	if (WIFEXITED(wstatus))
	{
		int	exit_status = WEXITSTATUS(wstatus);
		if (!exit_status)
			printf("Child Exited Normally!\n");
		else
			printf("Child Exited Abnormally!\nExit code: %d\n", WEXITSTATUS(wstatus));
		return (0);
	}
	return (1);
}

char	*strjoin(char *s1, char *s2)
{
	if (!s1 || !s2)
		return (0);
	size_t	s1_len = strlen(s1);
	size_t	s2_len = strlen(s2);
	char *res = malloc(s1_len + s2_len + 1);
	if (!res)
		return (0);
	*res = 0;
	strcat(res, s1);
	strcat(res, s2);
	res[s1_len + s2_len] = 0;
	return (res);
}

void	path_search_free(t_path_search *data)
{
	free(data->path);
	free(data->res);
	free(data->temp);
}

char	*search_cmd(char *cmd)
{
	t_path_search	data;

	memset(&data, 0, sizeof(data));
	data.path = getenv("PATH");
	if (!data.path)
	{
		fprintf(stderr, "PATH env not found!\n");
		return (0);
	}
	data.path = strdup(data.path);
	if (!data.path)
	{
		perror("malloc");
		return (0);
	}
	data.tok = strtok(data.path, ":");
	while (data.tok)
	{
		data.temp = strjoin(data.tok, "/");
		if (!data.temp)
		{
			path_search_free(&data);
			return (0);
		}
		data.res = strjoin(data.temp, cmd);
		free(data.temp);
		data.temp = 0;
		if (!data.res)
		{
			path_search_free(&data);
			return (0);
		}
		if (!access(data.res, F_OK))
		{
			if (access(data.res, X_OK))
			{
				perror(cmd);
				path_search_free(&data);
				return (0);
			}
			free(data.path);
			return (data.res);
		}
		free(data.res);
		data.tok = strtok(0, ":");
	}
	free(data.path);
	fprintf(stderr, "%s not found!\n", cmd);
	return (0);
}

int	check_cmd(char *cmd)
{
	char	*res;

	if (!strchr(cmd, '/'))
	{
		res = search_cmd(cmd);
		if (!res)
			return (0);
	}
	else
	{
		if (access(cmd, F_OK))
		{
			perror(cmd);
			return (0);
		}
		if (access(cmd, X_OK))
		{
			perror(cmd);
			return (0);
		}
	}
	return (1);
}

int main(int ac, char **av, char **ev)
{
	if (ac != 2)
	{
		fprintf(stderr, "Usage: tracer1 <exec path>\n");
		return (0);
	}
	if (!check_cmd(av[1]))
		return (1);
	int pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		return (1);
	}
	if (!pid)
	{
		char *args[2];
		args[0] = av[1];
		args[1] = 0;
		if (ptrace(PTRACE_TRACEME) == -1)
		{
			perror("ptrace error");
			return (1);
		}
		execve(av[1], args, ev);
		perror("execve error");
		return (1);
	}
	//char	breakpoint_ins = 0x33;

	if (!wait_child(pid))
		return (0);
	printf("Tracer Start!\n");
	while (1)
	{
		regs_dump(pid);
		char *line = readline("> ");
		if (!line)
		{
			if (ptrace(PTRACE_KILL, pid) == -1)
				perror("ptrace error");
			return (1);
		}
		if (ptrace(PTRACE_CONT, pid, 0, 0) == -1)
		{
			perror("ptrace error");
			return (1);
		}
		if (!wait_child(pid))
			return (0);
	}
	return (0);
}
