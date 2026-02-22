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
	char	**args;

	args = ft_split(cmd, ' ');
	if (!args)
		exit(1);
	if (!args[0])
	{
		dfree(args);
		exit(127);
	}
	path = get_path(args[0]);
	if (!path)
	{
		printf("command not found: %s\n", args[0]);
		dfree(args);
		exit(127);
	}
	execve(path, args, envp);
	perror(path);
	free(path);
	dfree(args);
	if (errno == ENOENT)
		exit(127);
	exit(126);
}