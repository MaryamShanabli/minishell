/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:30 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 15:16:11 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	get_cd_path(t_cmd *cmd, t_shell *shell, char **path)
{
	if (!cmd->argv[1] || !ft_strcmp(cmd->argv[1], "~")
		|| !ft_strcmp(cmd->argv[1], "--"))
	{
		*path = env_get(shell->env, "HOME");
		if (!*path || !**path)
		{
			error_msg(1, "cd", NULL, "HOME not set");
			return (1);
		}
		return (0);
	}
	if (cmd->argv[2])
	{
		error_msg(1, "cd", NULL, "too many arguments");
		return (1);
	}
	*path = cmd->argv[1];
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*oldpwd;
	char	*newpwd;

	if (get_cd_path(cmd, shell, &path))
		return (1);
	oldpwd = getcwd(NULL, 0);
	if (chdir(path) == -1)
	{
		free(oldpwd);
		error_msg(1, "cd", path, "No such file or directory");
		return (1);
	}
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		error_msg(1, "cd", "error retrieving current directory",
			strerror(errno));
	if (oldpwd)
		(void)env_set(&shell->env, "OLDPWD", oldpwd);
	if (newpwd)
		(void)env_set(&shell->env, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
	return (0);
}
