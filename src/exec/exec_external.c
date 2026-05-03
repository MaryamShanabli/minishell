/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 22:18:55 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 18:38:15 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	child_reset_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

static int	child_status(int cmd_status)
{
	int	status;

	status = 1;
	if (WIFEXITED(cmd_status))
		status = WEXITSTATUS(cmd_status);
	else if (WIFSIGNALED(cmd_status))
	{
		if (WTERMSIG(cmd_status) == SIGQUIT)
			write(2, "Quit\n", 5);
		else if (WTERMSIG(cmd_status) == SIGINT)
			write(2, "\n", 1);
		status = 128 + WTERMSIG(cmd_status);
	}
	return (status);
}

static int	fork_and_exec(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		cmd_status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
	{
		child_reset_signals();
		if (apply_redirections(cmd, shell))
			exit(1);
		exec_child(cmd, shell);
	}
	waitpid(pid, &cmd_status, 0);
	return (child_status(cmd_status));
}

int	execute_external(t_cmd *cmd, t_shell *shell)
{
	int					status;
	struct sigaction	sa_ign;
	struct sigaction	old_sa_int;
	struct sigaction	old_sa_quit;

	pipe_sig_setup(&sa_ign, &old_sa_int, &old_sa_quit);
	status = fork_and_exec(cmd, shell);
	sigaction(SIGINT, &old_sa_int, NULL);
	sigaction(SIGQUIT, &old_sa_quit, NULL);
	if (status == -1)
		return (1);
	return (status);
}
