/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:08:25 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/14 16:38:13 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exec_pipe(t_cmd *cmd, t_exec *exec, t_shell *shell)
{
	cmd->pid = fork();
	if (cmd->pid == -1)
		return (perror("fork"));
	if (cmd->pid == 0)
	{
		if (exec->prev_pipe_read != -1)
			dup2(exec->prev_pipe_read, STDIN_FILENO);
		if (cmd->next)
			dup2(exec->pipe_fd[1], STDOUT_FILENO);
		/* Plan: apply command redirections after pipe wiring so redir can override. */
		if (apply_redirections(cmd))
			exit(1);
		if (is_builtin(cmd->argv[0]))
			exit(execute_builtin(cmd, shell));
		else
			exec_child(cmd, shell);
	}
	if (exec->prev_pipe_read != -1)
		close(exec->prev_pipe_read);
	if (cmd->next)
	{
		close(exec->pipe_fd[1]);
		exec->prev_pipe_read = exec->pipe_fd[0];
	}
}

static int	wait_pipeline(t_cmd *first)
{
	int		cmd_status;
	int		status;
	t_cmd	*cmd;

	cmd = first;
	status = 0;
	while (cmd)
	{
		waitpid(cmd->pid, &cmd_status, 0);
		if (WIFEXITED(cmd_status))
			status = WEXITSTATUS(cmd_status);
		else if (WIFSIGNALED(cmd_status))
			status = 128 + WTERMSIG(cmd_status);
		cmd = cmd->next;
	}
	return (status);
}

int	execute_pipeline(t_cmd *cmd, t_shell *shell)
{
	t_exec	exec;
	t_cmd	*first;

	first = cmd;
	exec.in_fd = 0;
	exec.prev_pipe_read = -1;
	while (cmd)
	{
		if (cmd->next)
		{
			if (pipe(exec.pipe_fd) == -1)
				return (perror("pipe"), 1);
		}
		exec_pipe(cmd, &exec, shell);
		cmd = cmd->next;
	}
	return (wait_pipeline(first));
}
