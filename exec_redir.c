/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:14:33 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 15:14:34 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"
#include <fcntl.h>
#include <signal.h>

static int	do_heredoc(const char *delim)
{
	int		fd[2];
	char	*line;
	pid_t	pid;
	int		status;
	struct sigaction	sa;
	struct sigaction	old_int;
	struct sigaction	old_quit;

	if (pipe(fd) == -1)
		return (perror("pipe"), 1);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, &old_int);
	sigaction(SIGQUIT, &sa, &old_quit);
	pid = fork();
	if (pid == -1)
		return (sigaction(SIGINT, &old_int, NULL),
			sigaction(SIGQUIT, &old_quit, NULL),
			close(fd[0]), close(fd[1]), perror("fork"), 1);
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		close(fd[0]);
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
			write(fd[1], line, ft_strlen(line));
			write(fd[1], "\n", 1);
			free(line);
		}
		close(fd[1]);
		_exit(0);
	}
	close(fd[1]);
	waitpid(pid, &status, 0);
	sigaction(SIGINT, &old_int, NULL);
	sigaction(SIGQUIT, &old_quit, NULL);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		g_signal = SIGINT;
		write(1, "\n", 1);
		rl_cleanup_after_signal();
		rl_reset_after_signal();
		return (close(fd[0]), 130);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		g_signal = SIGINT;
		rl_cleanup_after_signal();
		rl_reset_after_signal();
		return (close(fd[0]), 130);
	}
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
	int		ret;

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
		ret = do_one_redir(redir);
		if (ret)
			return (close(base_stdin), ret);
		redir = redir->next;
	}
	close(base_stdin);
	return (0);
}
