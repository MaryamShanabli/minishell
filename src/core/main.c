/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 12:00:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/02 23:00:35 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	signal_handler(int signum)
{
	g_signal = signum;
	if (signum == SIGINT)
		write(1, "\n", 1);
}

static void	setup_signals(void)
{
	struct sigaction	sa;

	//rl_catch_signals = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

static int	run_non_interactive(char *input, t_shell *shell)
{
	t_cmd	cmd;
	int		status;

	cmd = process_input(input, shell);
	if (cmd.pid == -2)
		return (2);
	status = execute(&cmd, shell);
	shell->last_status = status;
	free_cmd_list(&cmd);
	return (status);
}

static void	update_shlvl(t_shell *shell)
{
	char	*val;
	long	lvl;
	char	buf[12];

	val = env_get(shell->env, "SHLVL");
	if (!val)
		lvl = 1;
	else
		lvl = ft_atol(val) + 1;
	if (lvl <= 0)
		lvl = 0;
	if (lvl >= 1000)
	{
		snprintf(buf, sizeof(buf), "%ld", lvl);
		write(2, "./minishell: warning: shell level (", 35);
		write(2, buf, ft_strlen(buf));
		write(2, ") too high, resetting to 1\n", 27);
		lvl = 1;
	}
	snprintf(buf, sizeof(buf), "%ld", lvl);
	env_set(&shell->env, "SHLVL", buf);
}

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;
	int		status;

	shell.env = env_dup(envp);
	if (!shell.env)
		return (1);
	shell.last_status = 0;
	update_shlvl(&shell);
	setup_signals();
	if (ac > 1)
		status = run_non_interactive(av[1], &shell);
	else
		status = interactive_loop(&shell);
	dfree(shell.env);
	exit(status);
}
