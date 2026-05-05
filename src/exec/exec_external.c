/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 22:18:55 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 17:08:24 by mshanabl         ###   ########.fr       */
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
	int		status;

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
	status = child_status(cmd_status);
	return (status);
}

void	pipe_sig_setup(t_pipe_sig *sig)
{
	sigemptyset(&sig->sa_ign.sa_mask);
	sig->sa_ign.sa_flags = 0;
	sig->sa_ign.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sig->sa_ign, &sig->old_int);
	sigaction(SIGQUIT, &sig->sa_ign, &sig->old_quit);
}

int	execute_external(t_cmd *cmd, t_shell *shell)
{
	int			status;
	t_pipe_sig	sig;

	pipe_sig_setup(&sig);
	status = fork_and_exec(cmd, shell);
	sigaction(SIGINT, &sig.old_int, NULL);
	sigaction(SIGQUIT, &sig.old_quit, NULL);
	if (status == -1)
		return (1);
	return (status);
}
