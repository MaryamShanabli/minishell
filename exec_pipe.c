/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:08:25 by mshanabl          #+#    #+#             */
/*   Updated: 2026/03/28 22:42:18 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <fcntl.h>

static t_redir	*is_redir(t_cmd *cmd, t_redir_type type)
{
	t_redir	*redir;

	if (!cmd || !cmd->redirs)
		return (NULL);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == type)
			return (redir);
		redir = redir->next;
	}
	return (NULL);
}

static void	exec_pipe(t_cmd *cmd, t_exec *exec, char **envp, int status)
{
	cmd->pid = fork();
	if (cmd->pid == -1)
		return (perror("fork"));
	if (cmd->pid == 0)
	{
		t_redir	*redir_in;
		t_redir	*redir_out;

		redir_in = is_redir(cmd, R_IN);
		if (redir_in)
		{
			exec->in_fd = open(redir_in->target, O_RDONLY);
			if (exec->in_fd == -1)
				exit(error_msg(127, redir_in->target, "operation not permitted"));
			dup2(exec->in_fd, STDIN_FILENO);
			close(exec->in_fd);
		}
		else if (exec->prev_pipe_read != -1)
			dup2(exec->prev_pipe_read, STDIN_FILENO);
		redir_out = is_redir(cmd, R_OUT);
		if (redir_out)
		{
			exec->out_fd = open(redir_out->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (exec->out_fd == -1)
				exit(error_msg(127, redir_out->target, "operation not permitted"));
			dup2(exec->out_fd, STDOUT_FILENO);
			close(exec->out_fd);
		}
		else if (cmd->next)
			dup2(exec->pipe_fd[1], STDOUT_FILENO);
		if (is_builtin(cmd->argv[0]))
			exit(execute_builtin(cmd, status));
		else
			exec_child(cmd, envp);
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

int	execute_pipeline(t_cmd *cmd, char **envp, int status)
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
		exec_pipe(cmd, &exec, envp, status);
		cmd = cmd->next;
	}
	return (wait_pipeline(first));
}
