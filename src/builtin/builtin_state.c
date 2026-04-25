/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:30 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/25 14:39:44 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

static void	print_export(const char *entry)
{
	char	*eq;

	eq = ft_strchr(entry, '=');
	write(1, "declare -x ", 11);
	if (!eq)
	{
		write(1, entry, ft_strlen(entry));
		write(1, "\n", 1);
		return ;
	}
	write(1, entry, (size_t)(eq - entry));
	write(1, "=\"", 2);
	write(1, eq + 1, ft_strlen(eq + 1));
	write(1, "\"\n", 2);
}

static int	export_one_arg(char *arg, t_shell *shell)
{
	char	*eq_pos;
	int		err;

	eq_pos = ft_strchr(arg, '=');
	err = 0;
	if (!eq_pos && !is_valid(arg))
		err = error_msg(1, "export", arg, "not a valid identifier");
	else if (!eq_pos && env_set(&shell->env, arg, NULL))
		err = 1;
	else if (eq_pos)
	{
		*eq_pos = '\0';
		if (!is_valid(arg))
			err = error_msg(1, "export", arg, "not a valid identifier");
		else if (env_set(&shell->env, arg, eq_pos + 1))
			err = 1;
		*eq_pos = '=';
	}
	return (err);
}

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
		error_msg(1, "cd", path, strerror(errno));
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

int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*arg;
	int		status;

	arg = cmd->argv[1];
	if (!arg)
	{
		i = 0;
		while (shell->env && shell->env[i])
		{
			print_export(shell->env[i]);
			i++;
		}
		return (0);
	}
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		arg = cmd->argv[i];
		if (export_one_arg(arg, shell))
			status = 1;
		i++;
	}
	return (status);
}

