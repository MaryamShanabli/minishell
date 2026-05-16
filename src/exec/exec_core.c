/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_core.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:49 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/12 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	exit(126);
}

static void	exec_path_lookup(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	int		saved_errno;

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

void	exec_child(t_cmd *cmd, t_shell *shell)
{
	if (ft_strchr(cmd->argv[0], '/'))
	{
		exec_slash_path(cmd, shell);
		return ;
	}
	exec_path_lookup(cmd, shell);
}

int	execute(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if (!cmd)
		return (shell->last_status);
	status = preprocess_heredocs(cmd, shell);
	if (status)
		return (status);
	if (cmd->next || !cmd->argv || !cmd->argv[0])
		return (execute_pipeline(cmd, shell));
	if (!ft_strcmp(cmd->argv[0], ".") && !cmd->argv[1])
	{
		write(2, "./minishell: .: filename argument required\n", 43);
		write(2, "./minishell: .: usage: . filename [arguments]\n", 47);
		return (2);
	}
	if (is_builtin(cmd->argv[0]))
	{
		status = execute_builtin_with_redir(cmd, shell);
		if (status != -1)
			return (status);
	}
	return (execute_external(cmd, shell));
}
