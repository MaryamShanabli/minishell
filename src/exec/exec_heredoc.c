/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 17:40:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/17 18:22:36 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	hd_child_wait(pid_t pid, int read_fd)
{
	int	ws;

	while (waitpid(pid, &ws, 0) == -1 && errno == EINTR)
		;
	if (WIFSIGNALED(ws) && WTERMSIG(ws) == SIGINT)
	{
		write(1, "\n", 1);
		close(read_fd);
		return (-2);
	}
	if (WIFEXITED(ws) && WEXITSTATUS(ws) == 1)
		write(2, "./minishell: warning: here-document delimited"
			" by end-of-file\n", 62);
	return (read_fd);
}

static int	hd_fork(int *fd, const char *delim, t_shell *shell, int expand)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		close(fd[0]);
		heredoc_loop(fd[1], delim, shell, expand);
	}
	close(fd[1]);
	return ((int)pid);
}

int	do_heredoc_pre(t_redir *redir, t_shell *shell)
{
	int				fd[2];
	int				pid;
	struct termios	old_term;
	int				have_term;

	if (pipe(fd) == -1)
		return (-1);
	have_term = (tcgetattr(STDIN_FILENO, &old_term) == 0);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = hd_fork(fd, redir->target, shell, redir->heredoc_expand);
	pid = hd_child_wait((pid_t)pid, fd[0]);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	if (have_term)
		tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
	if (pid >= 0)
		redir->heredoc_fd = pid;
	return (pid);
}

static int	process_cmd_redirs(t_cmd *cur, t_cmd *head, t_shell *shell)
{
	t_redir	*r;
	int		result;

	r = cur->redirs;
	while (r)
	{
		if (r->type == R_HEREDOC)
		{
			result = do_heredoc_pre(r, shell);
			if (result < 0)
			{
				close_heredoc_fds(head);
				if (result == -2)
					return (130);
				return (1);
			}
		}
		r = r->next;
	}
	close_heredoc_fds(head);
	return (0);
}

int	preprocess_heredocs(t_cmd *cmd, t_shell *shell)
{
	t_cmd	*cur;
	int		status;

	cur = cmd;
	while (cur)
	{
		status = process_cmd_redirs(cur, cmd, shell);
		if (status != 0)
			return (status);
		cur = cur->next;
	}
	return (0);
}
