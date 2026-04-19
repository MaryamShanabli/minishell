/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:59 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 15:13:00 by mshanabl         ###   ########.fr       */
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

static void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

static void	exec_pipe(t_cmd *cmd, t_exec *exec, t_shell *shell)
{
	cmd->pid = fork();
	if (cmd->pid == -1)
		return (perror("fork"));
	if (cmd->pid == 0)
	{
		set_child_signals();
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
			exit(130);
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
	pid_t	waited;
	t_cmd	*cmd;

	cmd = first;
	status = 0;
	while (cmd)
	{
		if (cmd->pid <= 0)
		{
			status = 1;
			cmd = cmd->next;
			continue ;
		}
		waited = waitpid(cmd->pid, &cmd_status, 0);
		if (waited == -1)
		{
			status = 1;
			cmd = cmd->next;
			continue ;
		}
		if (WIFEXITED(cmd_status))
			status = WEXITSTATUS(cmd_status);
		else if (WIFSIGNALED(cmd_status))
		{
			if (cmd->next == NULL && WTERMSIG(cmd_status) == SIGQUIT)
				write(2, "Quit: 3\n", 8);
			else if (cmd->next == NULL && WTERMSIG(cmd_status) == SIGINT)
				write(2, "\n", 1);
			status = 128 + WTERMSIG(cmd_status);
		}
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
	exec.pipe_fd[0] = -1;
	exec.pipe_fd[1] = -1;
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
