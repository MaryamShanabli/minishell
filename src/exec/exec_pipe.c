/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:59 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/16 12:49:07 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	wait_one(pid_t last_pid, int *status)
{
	int		ws;
	pid_t	reaped;

	reaped = waitpid(-1, &ws, 0);
	if (reaped == -1)
	{
		if (errno == EINTR)
			return (0);
		return (-1);
	}
	if (last_pid != -1 && reaped != last_pid)
		return (0);
	if (WIFEXITED(ws))
		*status = WEXITSTATUS(ws);
	else if (WIFSIGNALED(ws))
	{
		if (WTERMSIG(ws) == SIGQUIT && reaped == last_pid)
			write(2, "Quit\n", 5);
		*status = 128 + WTERMSIG(ws);
	}
	return (0);
}

static int	wait_pipeline(t_cmd *first)
{
	pid_t	last;
	int		status;

	last = -1;
	status = 0;
	while (first)
	{
		if (first->pid > 0)
			last = first->pid;
		first = first->next;
	}
	while (wait_one(last, &status) != -1)
		;
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
	if (g_signal == SIGINT)
		status = 130;
	return (status);
}

static void	pipe_fail(t_exec *exec, t_cmd *first, t_pipe_sig *sig)
{
	perror("pipe");
	if (exec->prev_pipe_read != -1)
		close(exec->prev_pipe_read);
	wait_pipeline(first);
	sigaction(SIGINT, &sig->old_int, NULL);
	sigaction(SIGQUIT, &sig->old_quit, NULL);
}

static void	pipeline_cleanup(t_cmd *first, t_pipe_sig *sig)
{
	close_heredoc_fds(first);
	sigaction(SIGINT, &sig->old_int, NULL);
	sigaction(SIGQUIT, &sig->old_quit, NULL);
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
	while (cmd)
	{
		if (cmd->next && pipe(exec.pipe_fd) == -1)
		{
			pipe_fail(&exec, first, &sig);
			return (1);
		}
		exec_pipe(cmd, &exec, shell);
		cmd = cmd->next;
	}
	pipeline_cleanup(first, &sig);
	status = wait_pipeline(first);
	return (status);
}
