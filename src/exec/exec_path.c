/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:55 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/25 14:39:44 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

static int	is_executable_file(const char *path)
{
	struct stat	st;

	if (stat(path, &st) == -1)
		return (0);
	if (!S_ISREG(st.st_mode))
		return (0);
	if (access(path, X_OK) == -1)
		return (0);
	return (1);
}

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
		if (is_executable_file(full))
			return (full);
		free(full);
		i++;
	}
	return (NULL);
}

char	*get_path(char *cmd, t_shell *shell)
{
	char			*env;
	char			**paths;
	char			*full;
	char			*path_src;
	const char		*default_path;

	default_path = "/usr/local/sbin:/usr/local/bin:/usr/sbin:"
		"/usr/bin:/sbin:/bin";
	env = env_get(shell->env, "PATH");
	if (!env || !*env)
		path_src = (char *)default_path;
	else
		path_src = env;
	paths = ft_split(path_src, ':');
	if (!paths)
		return (NULL);
	full = find_cmd_path(paths, cmd);
	dfree(paths);
	return (full);
}
