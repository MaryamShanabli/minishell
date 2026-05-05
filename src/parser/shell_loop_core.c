/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop_core.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 22:30:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/05 05:30:12 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_exit_cmd(t_cmd *cmd, t_shell *shell, char *input)
{
	int	raw;

	if (cmd->pid == -2 || !cmd->argv || cmd->next)
		return (-1);
	if (ft_strcmp(cmd->argv[0], "exit") != 0)
		return (-1);
	raw = shell->last_status;
	if (raw == 256)
	{
		shell->last_status = 1;
		return (0);
	}
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

static int	handle_input(t_shell *shell, char *input, const char *msg,
		size_t msg_len)
{
	int	cont;

	cont = read_quote_continuation(&input);
	if (cont != 0)
	{
		if (input)
			free(input);
		if (cont == 1)
			write(2, msg, msg_len);
		shell->last_status = 2;
		return (1);
	}
	cont = read_pipe_continuation(&input);
	if (cont != 0)
	{
		if (input)
			free(input);
		if (cont == 1)
			write(2, msg, msg_len);
		shell->last_status = 2;
		return (1);
	}
	consume_sigint(shell);
	return (process_line(input, shell) == 2);
}

int	prompt_loop(t_shell *shell, const char *msg, size_t msg_len)
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
		if (handle_input(shell, input, msg, msg_len))
			break ;
	}
	return (shell->last_status);
}
