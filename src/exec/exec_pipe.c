/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:59 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/02 22:37:55 by mshanabl         ###   ########.fr       */
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

static void	pipe_child(t_cmd *cmd, t_exec *exec, t_shell *shell)
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

int	execute_pipeline(t_cmd *cmd, t_shell *shell)
{
	t_exec				exec;
	t_cmd				*first;
	int					status;
	struct sigaction	sa_ign;
	struct sigaction	old_sa_int;
	struct sigaction	old_sa_quit;

	first = cmd;
	exec.in_fd = 0;
	exec.pipe_fd[0] = -1;
	exec.pipe_fd[1] = -1;
	exec.prev_pipe_read = -1;
	sigemptyset(&sa_ign.sa_mask);
	sa_ign.sa_flags = 0;
	sa_ign.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_ign, &old_sa_int);
	sigaction(SIGQUIT, &sa_ign, &old_sa_quit);
	while (cmd)
	{
		if (cmd->next && pipe(exec.pipe_fd) == -1)
		{
			perror("pipe");
			if (exec.prev_pipe_read != -1)
				close(exec.prev_pipe_read);
			wait_pipeline(first);
			sigaction(SIGINT, &old_sa_int, NULL);
			sigaction(SIGQUIT, &old_sa_quit, NULL);
			return (1);
		}
		exec_pipe(cmd, &exec, shell);
		cmd = cmd->next;
	}
	status = wait_pipeline(first);
	sigaction(SIGINT, &old_sa_int, NULL);
	sigaction(SIGQUIT, &old_sa_quit, NULL);
	return (status);
}
