/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 12:20:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/16 14:57:28 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <fcntl.h>

static int	do_heredoc(const char *delim)
{
	int		fd[2];
	char	*line;

	if (pipe(fd) == -1)
		return (perror("pipe"), 1);
	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, delim))
		{
			free(line);
			break ;
		}
		write(fd[1], line, ft_strlen(line));
		write(fd[1], "\n", 1);
		free(line);
	}
	close(fd[1]);
	if (dup2(fd[0], STDIN_FILENO) == -1)
		return (close(fd[0]), perror("dup2"), 1);
	close(fd[0]);
	return (0);
}

static int	do_one_redir(t_redir *redir)
{
	int	fd;

	if (redir->type == R_IN)
		fd = open(redir->target, O_RDONLY);
	else if (redir->type == R_OUT)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == R_APPEND)
		fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return (do_heredoc(redir->target));
	if (fd == -1)
		return (perror(redir->target), 1);
	if (redir->type == R_IN && dup2(fd, STDIN_FILENO) == -1)
		return (close(fd), perror("dup2"), 1);
	if ((redir->type == R_OUT || redir->type == R_APPEND)
		&& dup2(fd, STDOUT_FILENO) == -1)
		return (close(fd), perror("dup2"), 1);
	close(fd);
	return (0);
}

int	apply_redirections(t_cmd *cmd)
{
	t_redir	*redir;
	int		base_stdin;

	if (!cmd)
		return (1);
	base_stdin = dup(STDIN_FILENO);
	if (base_stdin == -1)
		return (perror("dup"), 1);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == R_HEREDOC)
		{
			if (dup2(base_stdin, STDIN_FILENO) == -1)
				return (close(base_stdin), perror("dup2"), 1);
		}
		if (do_one_redir(redir))
			return (close(base_stdin), 1);
		redir = redir->next;
	}
	close(base_stdin);
	return (0);
}
