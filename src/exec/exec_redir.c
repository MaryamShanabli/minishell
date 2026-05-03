/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:14:33 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 16:10:09 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	dup_or_fail(int fd, t_redir_type type)
{
	int	status;

	status = 0;
	if (type == R_IN && dup2(fd, STDIN_FILENO) == -1)
		status = 1;
	if ((type == R_OUT || type == R_APPEND)
		&& dup2(fd, STDOUT_FILENO) == -1)
		status = 1;
	if (status)
	{
		perror("dup2");
		return (1);
	}
	return (0);
}

static int	do_one_redir(t_redir *redir)
{
	int	fd;
	int	status;

	if (redir->type == R_IN)
		fd = open(redir->target, O_RDONLY);
	else if (redir->type == R_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		error_msg(1, redir->target, NULL, strerror(errno));
		return (1);
	}
	status = dup_or_fail(fd, redir->type);
	close(fd);
	if (status)
		return (1);
	return (0);
}

static int	reset_for_heredoc(int base_stdin, t_redir *redir)
{
	if (redir->type != R_HEREDOC)
		return (0);
	if (dup2(base_stdin, STDIN_FILENO) == -1)
	{
		perror("dup2");
		return (1);
	}
	return (0);
}

static int	apply_loop(t_redir *redir, int base_stdin, t_shell *shell)
{
	int	status;

	while (redir)
	{
		status = reset_for_heredoc(base_stdin, redir);
		if (!status)
		{
			if (redir->type == R_HEREDOC)
				status = do_heredoc(redir->target, shell);
			else
				status = do_one_redir(redir);
		}
		if (status)
			return (status);
		redir = redir->next;
	}
	return (0);
}

int	apply_redirections(t_cmd *cmd, t_shell *shell)
{
	int		base_stdin;
	int		status;

	if (!cmd)
		return (1);
	base_stdin = dup(STDIN_FILENO);
	if (base_stdin == -1)
	{
		perror("dup");
		return (1);
	}
	status = apply_loop(cmd->redirs, base_stdin, shell);
	if (status)
	{
		close(base_stdin);
		return (status);
	}
	close(base_stdin);
	return (0);
}
