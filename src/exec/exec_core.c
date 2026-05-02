/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_core.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:49 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/02 22:44:45 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

int	child_status(int cmd_status)
{
	int	status;

	status = 1;
	if (WIFEXITED(cmd_status))
		status = WEXITSTATUS(cmd_status);
	else if (WIFSIGNALED(cmd_status))
	{
		if (WTERMSIG(cmd_status) == SIGQUIT)
			write(2, "Quit\n", 5);
		else if (WTERMSIG(cmd_status) == SIGINT)
			write(2, "\n", 1);
		status = 128 + WTERMSIG(cmd_status);
	}
	return (status);
}

static int	exec_no_argv(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if ((!cmd->argv || !cmd->argv[0]) && cmd->redirs)
	{
		status = execute_redir_only(cmd);
		if (status)
			return (status);
		if (cmd->next)
		{
			status = execute_pipeline(cmd->next, shell);
			return (status);
		}
		return (0);
	}
	if (!cmd->argv || !cmd->argv[0])
		return (shell->last_status);
	return (-1);
}

static void	exec_slash_path(t_cmd *cmd, t_shell *shell)
{
	struct stat	st;
	int			saved_errno;

	if (stat(cmd->argv[0], &st) == 0 && S_ISDIR(st.st_mode))
	{
		error_msg(126, cmd->argv[0], NULL, "Is a directory");
		exit(126);
	}
	execve(cmd->argv[0], cmd->argv, shell->env);
	saved_errno = errno;
	error_msg(0, cmd->argv[0], NULL, strerror(saved_errno));
	if (saved_errno == ENOENT)
		exit(127);
	if (saved_errno == EACCES)
		exit(126);
	exit(126);
}

void	exec_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	int		saved_errno;

	if (ft_strcmp(cmd->argv[0], ".") == 0)
	{
		write(2, "./minishell: .: filename argument required\n", 43);
		write(2, "./minishell: .: usage: . filename [arguments]\n", 47);
		exit(2);
	}
	if (ft_strchr(cmd->argv[0], '/'))
	{
		exec_slash_path(cmd, shell);
		return ;
	}
	path = get_path(cmd->argv[0], shell);
	if (!path)
	{
		error_msg(127, cmd->argv[0], NULL, "command not found");
		exit(127);
	}
	execve(path, cmd->argv, shell->env);
	saved_errno = errno;
	error_msg(0, path, NULL, strerror(saved_errno));
	free(path);
	if (saved_errno == ENOENT)
		exit(127);
	exit(126);
}

int	execute(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if (!cmd)
		return (shell->last_status);
	status = exec_no_argv(cmd, shell);
	if (status != -1)
		return (status);
	if (cmd->next)
	{
		status = execute_pipeline(cmd, shell);
		return (status);
	}
	if (is_builtin(cmd->argv[0]))
	{
		status = execute_builtin_with_redir(cmd, shell);
		if (status != -1)
			return (status);
	}
	status = execute_external(cmd, shell);
	return (status);
}
