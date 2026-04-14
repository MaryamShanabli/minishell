/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:19 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/14 16:38:13 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	cd_getcwd_error(void)
{
	char	*msg;
	char	*full;

	msg = strerror(errno);
	full = ft_strjoin("getcwd: cannot access parent directories: ", msg);
	if (!full)
	{
		error_msg(1, "cd", "error retrieving current directory", msg);
		return ;
	}
	error_msg(1, "cd", "error retrieving current directory", full);
	free(full);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*target;
	char	*oldpwd;
	char	*newpwd;
	int		warned;

	warned = 0;
	if (cmd->argv[2])
		return (error_msg(1, "cd", NULL, "too many arguments"));
	if (!cmd->argv[1]
		|| !ft_strcmp(cmd->argv[1], "~")
		|| !ft_strcmp(cmd->argv[1], "--"))
	{
		target = env_get(shell->env, "HOME");
		if (!target || !*target)
			return (error_msg(1, "cd", NULL, "HOME not set"));
	}
	else
		target = cmd->argv[1];
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd && !warned)
	{
		cd_getcwd_error();
		warned = 1;
	}
	if (chdir(target) == -1)
		return (free(oldpwd), error_msg(1, "cd", target, strerror(errno)));
	newpwd = getcwd(NULL, 0);
	if (!newpwd && !warned)
	{
		cd_getcwd_error();
		warned = 1;
	}
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
	char	*eq_pos;
	int		status;

	arg = cmd->argv[1];
	if (!arg)
	{
		i = 0;
		while (shell->env && shell->env[i])
		{
			write(1, "declare -x ", 11);
			write(1, shell->env[i], ft_strlen(shell->env[i]));
			write(1, "\n", 1);
			i++;
		}
		return (0);
	}
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		arg = cmd->argv[i];
		eq_pos = strchr(arg, '=');
		if (eq_pos)
		{
			*eq_pos = '\0';
			if (env_set(&shell->env, arg, eq_pos + 1))
				status = 1;
			*eq_pos = '=';
		}
		else if (env_set(&shell->env, arg, NULL))
			status = 1;
		i++;
	}
	return (status);
}

int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	int		status;
	char	*arg;

	if (!cmd || !cmd->argv || !cmd->argv[1])
		return (0);
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		arg = cmd->argv[i];
		if (!is_valid(arg))
		{
			error_msg(1, "unset", arg, "not a valid identifier");
			status = 1;
		}
		else
			(void)env_unset(&shell->env, arg);
		i++;
	}
	return (status);
}
