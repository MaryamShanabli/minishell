#include "minishell.h"

char	*get_path(char *cmd)
{
	char	*env;
	char	**paths;
	char	*full;
	char	*tmp;
	int		i;

	env = getenv("PATH");
	paths = ft_split(env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(full, X_OK) == 0)
		{
			dfree(paths);
			return (full);
		}
		free(full);
		i++;
	}
	dfree(paths);
	return (NULL);
}
void	execute(char *cmd, char **envp)
{
	char	*path;
	t_cmd	command;

	command.argv = ft_split(cmd, ' ');
	if (!command.argv)
		exit(1);
	if (!command.argv[0])
	{
		dfree(command.argv);
		exit(127);
	}
	if (command.is_builtin)
	{
		// Handle built-in commands here (not implemented in this snippet)
		dfree(command.argv);
		exit(0);
	}
	path = get_path(command.argv[0]);
	if (!path)
	{
		printf("command not found: %s\n", command.argv[0]);
		dfree(command.argv);
		exit(127);
	}
	execve(path, command.argv, envp);
	perror(path);
	free(path);
	dfree(command.argv);
	if (errno == ENOENT)
		exit(127);
	exit(126);
}

void exit_msg(int exit_code, const char *msg)
{
	perror(msg);
	exit(exit_code);
}