/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc_loop.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 14:02:57 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 20:21:08 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	heredoc_write_line(int write_fd, char *line, t_shell *shell,
			int expand)
{
	char	*expanded;

	if (expand)
	{
		expanded = expand_one(line, shell);
		free(line);
		if (!expanded)
		{
			close(write_fd);
			_exit(1);
		}
		write(write_fd, expanded, ft_strlen(expanded));
		free(expanded);
	}
	else
	{
		write(write_fd, line, ft_strlen(line));
		free(line);
	}
	write(write_fd, "\n", 1);
}

void	heredoc_loop(int write_fd, const char *delim, t_shell *shell,
			int expand)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			close(write_fd);
			_exit(1);
		}
		if (!ft_strcmp(line, delim))
		{
			free(line);
			break ;
		}
		heredoc_write_line(write_fd, line, shell, expand);
	}
	close(write_fd);
	_exit(0);
}
