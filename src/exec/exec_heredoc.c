/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 17:40:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 20:21:08 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	heredoc_status(int wait_status, int read_fd)
{
	if (WIFSIGNALED(wait_status) && WTERMSIG(wait_status) == SIGINT)
	{
		write(1, "\n", 1);
		close(read_fd);
		return (130);
	}
	if (WIFEXITED(wait_status) && WEXITSTATUS(wait_status) == 130)
	{
		close(read_fd);
		return (130);
	}
	if (WIFEXITED(wait_status) && WEXITSTATUS(wait_status) == 1)
	{
		write(2, "./minishell: warning: here-document delimited"
			" by end-of-file\n", 62);
		return (0);
	}
	return (0);
}

static int	hd_wait_and_restore(t_hd_finish *ctx)
{
	int	wait_status;
	int	status;

	close(ctx->fd[1]);
	while (waitpid(ctx->pid, &wait_status, 0) == -1)
	{
		if (errno != EINTR)
		{
			signal(SIGINT, ctx->old_int);
			signal(SIGQUIT, ctx->old_quit);
			close(ctx->fd[0]);
			perror("waitpid");
			return (-1);
		}
	}
	signal(SIGINT, ctx->old_int);
	signal(SIGQUIT, ctx->old_quit);
	tcsetattr(STDIN_FILENO, TCSANOW, &ctx->old_term);
	status = heredoc_status(wait_status, ctx->fd[0]);
	return (status);
}

static int	hd_dup_stdin(int read_fd)
{
	if (dup2(read_fd, STDIN_FILENO) == -1)
	{
		close(read_fd);
		perror("dup2");
		return (1);
	}
	close(read_fd);
	return (0);
}

static int	hd_fork(t_hd_finish *ctx, const char *delim, t_shell *shell,
			int expand)
{
	ctx->pid = fork();
	if (ctx->pid == -1)
	{
		signal(SIGINT, ctx->old_int);
		signal(SIGQUIT, ctx->old_quit);
		close(ctx->fd[0]);
		close(ctx->fd[1]);
		return (1);
	}
	if (ctx->pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		close(ctx->fd[0]);
		heredoc_loop(ctx->fd[1], delim, shell, expand);
	}
	return (0);
}

int	do_heredoc(const char *delim, t_shell *shell, int expand)
{
	t_hd_finish	ctx;
	int			status;

	if (tcgetattr(STDIN_FILENO, &ctx.old_term) == -1)
		ft_bzero(&ctx.old_term, sizeof(ctx.old_term));
	if (pipe(ctx.fd) == -1)
		return (1);
	ctx.old_int = signal(SIGINT, SIG_IGN);
	ctx.old_quit = signal(SIGQUIT, SIG_IGN);
	if (hd_fork(&ctx, delim, shell, expand))
		return (1);
	status = hd_wait_and_restore(&ctx);
	if (status)
		return (status);
	status = hd_dup_stdin(ctx.fd[0]);
	return (status);
}
