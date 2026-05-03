/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/03 18:26:19 by mshanabl         ###   ########.fr       */
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

static int	check_exit_cmd(t_cmd *cmd, t_shell *shell, char *input)
{
	int	raw;

	if (cmd->pid == -2 || !cmd->argv || cmd->next)
		return (-1);
	if (ft_strcmp(cmd->argv[0], "exit") != 0)
		return (-1);
	raw = shell->last_status;
	if (raw == 1)
		return (0);
	if (raw < 0)
		shell->last_status = -raw;
	(void)input;
	return (2);
}

static int	process_line(char *input, t_shell *shell)
{
	t_cmd	cmd;
	int		ret;

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
	ret = check_exit_cmd(&cmd, shell, input);
	free_cmd_list(&cmd);
	free(input);
	if (ret >= 0)
		return (ret);
	return (0);
}

static void	refresh_after_sigint(t_shell *shell)
{
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	consume_sigint(shell);
}

int	interactive_loop(t_shell *shell)
{
	char	*input;

	while (1)
	{
		if (g_signal == SIGINT)
			refresh_after_sigint(shell);
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
