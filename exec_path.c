#include "minishell.h"

static char	*find_cmd_path(char **paths, char *cmd)
{
	char	*full;
	char	*tmp;
	int		i;

	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(full, X_OK) == 0)
			return (full);
		free(full);
		i++;
	}
	return (NULL);
}

char	*get_path(char *cmd)
{
	char	*env;
	char	**paths;
	char	*full;

	env = getenv("PATH");
	if (!env)
		return (NULL);
	paths = ft_split(env, ':');
	if (!paths)
		return (NULL);
	full = find_cmd_path(paths, cmd);
	dfree(paths);
	return (full);
}
