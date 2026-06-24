/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 12:00:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/06/24 13:44:49 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	signal_handler(int signum)
{
	g_signal = signum;
	if (signum == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

static void	setup_signals(void)
{
	struct sigaction	sa;

	rl_catch_signals = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

static int	run_non_interactive(char *input, t_shell *shell)
{
	t_cmd	*cmd;
	int		status;

	cmd = process_input(input, shell);
	if (!cmd)
		return (2);
	status = execute(cmd, shell);
	shell->last_status = status;
	free_cmd_list(cmd);
	return (status);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;
	int		status;

	print_minishell_masterpiece();
	shell.env = env_dup(envp);
	if (!shell.env)
		return (1);
	shell.last_status = 0;
	shell.should_exit = 0;
	shell.exit_code = 0;
	update_shlvl(&shell);
	setup_signals();
	if (ac > 1)
		status = run_non_interactive(av[1], &shell);
	else
		status = interactive_loop(&shell);
	dfree(shell.env);
	exit(status);
}
