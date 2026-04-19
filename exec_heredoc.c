/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 17:40:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 18:01:28 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>

static void	heredoc_loop(int write_fd, const char *delim)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (!ft_strcmp(line, delim))
		{
			free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
	close(write_fd);
	_exit(0);
}

static int	heredoc_status(int wait_status, int read_fd)
{
	if (WIFSIGNALED(wait_status) && WTERMSIG(wait_status) == SIGINT)
	{
		g_signal = SIGINT;
		write(1, "\n", 1);
		rl_cleanup_after_signal();
		rl_reset_after_signal();
		close(read_fd);
		return (130);
	}
	if (WIFEXITED(wait_status) && WEXITSTATUS(wait_status) == 130)
	{
		g_signal = SIGINT;
		rl_cleanup_after_signal();
		rl_reset_after_signal();
		close(read_fd);
		return (130);
	}
	return (0);
}

static int	heredoc_setup(int fd[2], pid_t *pid, void (**old_int)(int),
	void (**old_quit)(int))
{
	if (pipe(fd) == -1)
	{
		perror("pipe");
		return (1);
	}
	*old_int = signal(SIGINT, SIG_IGN);
	*old_quit = signal(SIGQUIT, SIG_IGN);
	*pid = fork();
	if (*pid == -1)
	{
		signal(SIGINT, *old_int);
		signal(SIGQUIT, *old_quit);
		close(fd[0]);
		close(fd[1]);
		perror("fork");
		return (1);
	}
	return (0);
}

static int	heredoc_finish(pid_t pid, int fd[2], void (*old_int)(int),
	void (*old_quit)(int))
{
	int	status;

	close(fd[1]);
	waitpid(pid, &status, 0);
	signal(SIGINT, old_int);
	signal(SIGQUIT, old_quit);
	status = heredoc_status(status, fd[0]);
	if (status)
		return (status);
	if (dup2(fd[0], STDIN_FILENO) == -1)
	{
		close(fd[0]);
		perror("dup2");
		return (1);
	}
	close(fd[0]);
	return (0);
}

int	do_heredoc(const char *delim)
{
	int		fd[2];
	pid_t	pid;
	int		status;
	void	(*old_int)(int);
	void	(*old_quit)(int);

	status = heredoc_setup(fd, &pid, &old_int, &old_quit);
	if (status)
		return (status);
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		close(fd[0]);
		heredoc_loop(fd[1], delim);
	}
	status = heredoc_finish(pid, fd, old_int, old_quit);
	return (status);
}
