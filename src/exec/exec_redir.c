/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:14:33 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/16 12:37:58 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	close_heredoc_fds(t_cmd *cmd)
{
	t_cmd	*cur;
	t_redir	*r;

	cur = cmd;
	while (cur)
	{
		r = cur->redirs;
		while (r)
		{
			if (r->type == R_HEREDOC && r->heredoc_fd > 0)
			{
				close(r->heredoc_fd);
				r->heredoc_fd = -1;
			}
			r = r->next;
		}
		cur = cur->next;
	}
}

static int	open_redir_fd(t_redir *redir)
{
	int	fd;
	int	flags;

	if (redir->type == R_IN)
		fd = open(redir->target, O_RDONLY);
	else
	{
		flags = O_WRONLY | O_CREAT;
		if (redir->type == R_OUT)
			flags |= O_TRUNC;
		else
			flags |= O_APPEND;
		fd = open(redir->target, flags, 0644);
	}
	if (fd == -1)
		error_msg(1, redir->target, NULL, strerror(errno));
	return (fd);
}

static int	do_one_redir(t_redir *redir)
{
	int	fd;
	int	target;

	if (!redir->target || !redir->target[0])
	{
		if (redir->target_was_unquoted_var)
			write(2, "minishell: ambiguous redirect\n", 30);
		else
			error_msg(1, "", NULL, "No such file or directory");
		return (1);
	}
	fd = open_redir_fd(redir);
	if (fd == -1)
		return (1);
	target = STDOUT_FILENO;
	if (redir->type == R_IN)
		target = STDIN_FILENO;
	if (dup2(fd, target) == -1)
	{
		perror("dup2");
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

static int	apply_heredoc_redir(t_redir *redir)
{
	if (redir->heredoc_fd < 0)
		return (1);
	if (dup2(redir->heredoc_fd, STDIN_FILENO) == -1)
	{
		perror("dup2");
		return (1);
	}
	close(redir->heredoc_fd);
	redir->heredoc_fd = -1;
	return (0);
}

int	apply_redirections(t_cmd *cmd)
{
	t_redir	*redir;
	int		status;

	if (!cmd)
		return (1);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == R_HEREDOC)
			status = apply_heredoc_redir(redir);
		else
			status = do_one_redir(redir);
		if (status)
			return (status);
		redir = redir->next;
	}
	return (0);
}
