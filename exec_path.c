/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:42 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/14 16:38:13 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

char	*get_path(char *cmd, t_shell *shell)
{
	char	*env;
	char	**paths;
	char	*full;

	env = env_get(shell->env, "PATH");
	if (!env)
		return (NULL);
	paths = ft_split(env, ':');
	if (!paths)
		return (NULL);
	full = find_cmd_path(paths, cmd);
	dfree(paths);
	return (full);
}
