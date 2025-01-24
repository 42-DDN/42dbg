#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>

int	regs_dump(pid_t pid)
{
	struct user_regs_struct regs;
	if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)
	{
		perror("ptrace error");
		return (EXIT_FAILURE);
	}
	printf("Simple registeries dump:\n");
	printf("\e[93mRIP: %llx\n", regs.rip);
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

int main(int ac, char **av, char **ev)
{
	if (ac != 2)
	{
		fprintf(stderr, "Usage: tracer1 <exec path>\n");
		return (0);
	}
	if (access(av[1], X_OK))
	{
		perror("permission error");
		return (1);
	}
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
	void	*data;
	void	*addr;
	char	breakpoint_ins = 0x33;

	if (!wait_child(pid))
		return (0);
	printf("Tracer Start!\n");
	regs_dump(pid);
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
