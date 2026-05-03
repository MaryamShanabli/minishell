/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 18:01:43 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 18:20:30 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	close_fd(int *fd)
{
	if (*fd != -1)
	{
		close(*fd);
		*fd = -1;
	}
}

void	pipe_child(t_cmd *cmd, t_exec *exec, t_shell *shell)
{
	int	status;

	child_reset_signals();
	if (exec->prev_pipe_read != -1)
	{
		dup2(exec->prev_pipe_read, STDIN_FILENO);
		close_fd(&exec->prev_pipe_read);
	}
	if (cmd->next)
		dup2(exec->pipe_fd[1], STDOUT_FILENO);
	close_fd(&exec->pipe_fd[0]);
	close_fd(&exec->pipe_fd[1]);
	if (apply_redirections(cmd))
		exit(1);
	if (is_builtin(cmd->argv[0]))
	{
		status = execute_builtin(cmd, shell);
		exit(status);
	}
	exec_child(cmd, shell);
}

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
