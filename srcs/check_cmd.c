#include <utils.h>

char	*path_search_free(t_path_search *data, int mem_err)
{
	if (mem_err)
		perror("malloc");
	free(data->path);
	free(data->res);
	free(data->temp);
	return (0);
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
			return (path_search_free(&data, 1));
		data.res = strjoin(data.temp, cmd);
		if (!data.res)
			return (path_search_free(&data, 1));
		if (!access(data.res, F_OK))
		{
			if (access(data.res, X_OK))
			{
				perror(cmd);
				return (path_search_free(&data, 0));
			}
			free(data.path);
			return (data.res);
		}
		free(data.res);
		free(data.temp);
		data.tok = strtok(0, ":");
	}
	free(data.path);
	fprintf(stderr, "%s not found!\n", cmd);
	return (0);
}

t_check_cmd	check_cmd(char *cmd)
{
	t_check_cmd	data;

	memset(&data, 0, sizeof(data));
	if (!strchr(cmd, '/'))
	{
		data.cmd_dir = search_cmd(cmd);
		if (!data.cmd_dir)
			return (data);
	}
	else
	{
		if (access(cmd, F_OK) || access(cmd, X_OK))
		{
			perror(cmd);
			return (data);
		}
		data.cmd_dir = strdup(cmd);
		if (!data.cmd_dir)
		{
			perror("malloc");
			return (data);
		}
	}
	data.is_success = 1;
	return (data);
}
