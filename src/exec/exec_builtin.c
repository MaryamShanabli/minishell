/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 16:45:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/12 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	restore_stdio(int saved_in, int saved_out)
{
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
}

int	is_builtin(char *name)
{
	if (!name)
		return (0);
	if (!ft_strcmp(name, "echo") || !ft_strcmp(name, "cd")
		|| !ft_strcmp(name, "pwd") || !ft_strcmp(name, "export")
		|| !ft_strcmp(name, "unset") || !ft_strcmp(name, "env")
		|| !ft_strcmp(name, "exit"))
		return (1);
	return (0);
}

int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	int	status;

	status = 1;
	if (!ft_strcmp(cmd->argv[0], "echo"))
		status = builtin_echo(cmd);
	else if (!ft_strcmp(cmd->argv[0], "cd"))
		status = builtin_cd(cmd, shell);
	else if (!ft_strcmp(cmd->argv[0], "pwd"))
		status = builtin_pwd(cmd);
	else if (!ft_strcmp(cmd->argv[0], "export"))
		status = builtin_export(cmd, shell);
	else if (!ft_strcmp(cmd->argv[0], "unset"))
		status = builtin_unset(cmd, shell);
	else if (!ft_strcmp(cmd->argv[0], "env"))
	{
		if (cmd->argv[1])
			return (-1);
		status = builtin_env(cmd, shell);
	}
	else if (!ft_strcmp(cmd->argv[0], "exit"))
		status = builtin_exit(cmd, shell);
	return (status);
}

int	execute_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
	int	saved_in;
	int	saved_out;
	int	status;

	saved_in = dup(STDIN_FILENO);
	if (saved_in == -1)
	{
		perror("dup");
		return (1);
	}
	saved_out = dup(STDOUT_FILENO);
	if (saved_out == -1)
	{
		perror("dup");
		close(saved_in);
		return (1);
	}
	status = apply_redirections(cmd);
	if (!status)
		status = execute_builtin(cmd, shell);
	restore_stdio(saved_in, saved_out);
	return (status);
}
