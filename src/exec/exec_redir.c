/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:14:33 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/05 03:10:49 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
				status = do_heredoc(redir->target, shell,
						redir->heredoc_expand);
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
