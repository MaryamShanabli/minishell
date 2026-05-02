/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/02 22:41:14 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	consume_sigint(t_shell *shell)
{
	if (g_signal == SIGINT)
	{
		g_signal = 0;
		if (shell)
			shell->last_status = 130;
		return (1);
	}
	return (0);
}

static void	handle_eof(int status)
{
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	(void)status;
}

static int	process_line(char *input, t_shell *shell)
{
	t_cmd	cmd;
	int		raw;

	if (!*input)
	{
		free(input);
		return (0);
	}
	add_history(input);
	cmd = process_input(input, shell);
	if (cmd.pid == -2)
		shell->last_status = 2;
	else
		shell->last_status = execute(&cmd, shell);
	if (cmd.pid != -2 && cmd.argv && !cmd.next
		&& !ft_strcmp(cmd.argv[0], "exit"))
	{
		raw = shell->last_status;
		if (raw == 1)
		{
			free_cmd_list(&cmd);
			free(input);
			return (0);
		}
		if (raw < 0)
			shell->last_status = -raw;
		free_cmd_list(&cmd);
		free(input);
		return (2);
	}
	free_cmd_list(&cmd);
	free(input);
	return (0);
}

int	interactive_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		if (g_signal == SIGINT)
		{
			rl_on_new_line();
			rl_replace_line("", 0);
			rl_redisplay();
			consume_sigint(shell);
		}
		input = readline("minishell$ ");
		if (!input)
		{
			if (consume_sigint(shell))
				continue ;
			handle_eof(shell->last_status);
			break ;
		}
		if (g_signal == SIGINT && !*input)
		{
			consume_sigint(shell);
			free(input);
			continue ;
		}
		consume_sigint(shell);
		if (process_line(input, shell) == 2)
			break ;
	}
	return (shell->last_status);
}
