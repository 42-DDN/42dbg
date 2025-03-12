#include <dbg.h>

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

t_dbg	prep_dbg(int ac, char **av)
{
	t_dbg	dbg_data;

	dbg_data.cmd = av[1];
	if (ac != 2)
	{
		fprintf(stderr, "Usage: 42dbg <exec path>\n");
		exit(EXIT_FAILURE);
	}
	t_check_cmd check_data = check_cmd(av[1]);
	if (!check_data.is_success)
		exit(EXIT_FAILURE);
	dbg_data.cmd_dir = check_data.cmd_dir;
	return (dbg_data);
}

int main(int ac, char **av, char **ev)
{
	t_dbg dbg_data = prep_dbg(ac, av);
	int pid = fork();
	if (pid == -1)
	{
		perror("fork error");
		return (1);
	}
	if (!pid)
	{
		char *args[2];
		args[0] = dbg_data.cmd;
		args[1] = 0;
		if (ptrace(PTRACE_TRACEME) == -1)
		{
			perror("ptrace error");
			free(dbg_data.cmd_dir);
			return (EXIT_FAILURE);
		}
		execve(dbg_data.cmd_dir, args, ev);
		perror("execve error");
		free(dbg_data.cmd_dir);
		return (EXIT_FAILURE);
	}
	//char	breakpoint_ins = 0x33;

	if (!wait_child(pid))
	{
		free(dbg_data.cmd_dir);
		return (0);
	}
	printf("Tracer Start!\n");
	while (1)
	{
		regs_dump(pid);
		char *line = readline("> ");
		if (!line)
		{
			if (ptrace(PTRACE_KILL, pid) == -1)
				perror("ptrace error");
			free(dbg_data.cmd_dir);
			return (1);
		}
		if (ptrace(PTRACE_CONT, pid, 0, 0) == -1)
		{
			perror("ptrace error");
			free(dbg_data.cmd_dir);
			return (1);
		}
		if (!wait_child(pid))
		{
			free(dbg_data.cmd_dir);
			return (0);
		}
	}
}
