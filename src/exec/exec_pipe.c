/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:59 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 18:34:18 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	exec_pipe(t_cmd *cmd, t_exec *exec, t_shell *shell)
{
	cmd->pid = fork();
	if (cmd->pid == -1)
	{
		perror("fork");
		cmd->pid = 0;
		return ;
	}
	if (cmd->pid == 0)
		pipe_child(cmd, exec, shell);
	if (exec->prev_pipe_read != -1)
		close(exec->prev_pipe_read);
	exec->prev_pipe_read = -1;
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
		if (cmd->pid <= 0 || waitpid(cmd->pid, &cmd_status, 0) == -1)
			status = 1;
		else if (WIFEXITED(cmd_status))
			status = WEXITSTATUS(cmd_status);
		else if (WIFSIGNALED(cmd_status))
		{
			if (cmd->next == NULL && WTERMSIG(cmd_status) == SIGQUIT)
				write(2, "Quit\n", 5);
			else if (cmd->next == NULL && WTERMSIG(cmd_status) == SIGINT)
				write(2, "\n", 1);
			status = 128 + WTERMSIG(cmd_status);
		}
		cmd = cmd->next;
	}
	return (status);
}

static int	run_pipe_loop(t_cmd *cmd, t_cmd *first, t_exec *exec,
	t_shell *shell)
{
	while (cmd)
	{
		if (cmd->next && pipe(exec->pipe_fd) == -1)
		{
			perror("pipe");
			if (exec->prev_pipe_read != -1)
				close(exec->prev_pipe_read);
			wait_pipeline(first);
			return (-1);
		}
		exec_pipe(cmd, exec, shell);
		cmd = cmd->next;
	}
	return (0);
}

int	execute_pipeline(t_cmd *cmd, t_shell *shell)
{
	t_exec		exec;
	t_cmd		*first;
	t_pipe_sig	sig;
	int			status;

	first = cmd;
	exec.in_fd = 0;
	exec.pipe_fd[0] = -1;
	exec.pipe_fd[1] = -1;
	exec.prev_pipe_read = -1;
	pipe_sig_setup(&sig);
	if (run_pipe_loop(cmd, first, &exec, shell) == -1)
	{
		sigaction(SIGINT, &sig.old_int, NULL);
		sigaction(SIGQUIT, &sig.old_quit, NULL);
		return (1);
	}
	sigaction(SIGINT, &sig.old_int, NULL);
	sigaction(SIGQUIT, &sig.old_quit, NULL);
	status = wait_pipeline(first);
	return (status);
}
