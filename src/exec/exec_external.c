/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 22:18:55 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/02 22:44:14 by mshanabl         ###   ########.fr       */
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

int	execute_external(t_cmd *cmd, t_shell *shell)
{
	pid_t				pid;
	int					cmd_status;
	struct sigaction	sa_ign;
	struct sigaction	old_sa_int;
	struct sigaction	old_sa_quit;

	sigemptyset(&sa_ign.sa_mask);
	sa_ign.sa_flags = 0;
	sa_ign.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_ign, &old_sa_int);
	sigaction(SIGQUIT, &sa_ign, &old_sa_quit);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		sigaction(SIGINT, &old_sa_int, NULL);
		sigaction(SIGQUIT, &old_sa_quit, NULL);
		return (1);
	}
	if (pid == 0)
	{
		child_reset_signals();
		if (apply_redirections(cmd))
			exit(1);
		exec_child(cmd, shell);
	}
	waitpid(pid, &cmd_status, 0);
	sigaction(SIGINT, &old_sa_int, NULL);
	sigaction(SIGQUIT, &old_sa_quit, NULL);
	return (child_status(cmd_status));
}
