/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/21 16:11:17 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_signal_check(void)
{
	if (g_signal == SIGINT)
	{
		g_signal = 0;
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
	if (cmd.argv && !cmd.next && !ft_strcmp(cmd.argv[0], "exit"))
	{
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
		if (handle_signal_check())
			continue ;
		input = readline("minishell$ ");
		if (!input)
		{
			if (handle_signal_check())
				continue ;
			handle_eof(shell->last_status);
			break ;
		}
		if (handle_signal_check())
		{
			free(input);
			continue ;
		}
		if (process_line(input, shell) == 2)
			break ;
	}
	return (shell->last_status);
}
