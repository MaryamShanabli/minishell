/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop_core.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 22:30:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_line(char *input, t_shell *shell)
{
	t_cmd	*cmd;

	if (!*input)
	{
		free(input);
		return (0);
	}
	add_history(input);
	cmd = process_input(input, shell);
	if (!cmd)
		shell->last_status = 2;
	else
	{
		shell->last_status = execute(cmd, shell);
		free_cmd_list(cmd);
	}
	free(input);
	if (shell->should_exit)
		return (2);
	return (0);
}

static int	check_continuation(t_shell *shell, char **input)
{
	int		cont;
	char	*err;

	err = "./minishell: syntax error: unexpected end of file\n";
	cont = read_quote_continuation(input);
	if (cont == 0)
		cont = read_pipe_continuation(input);
	if (cont == -1)
	{
		if (*input)
			free(*input);
		shell->last_status = 130;
		return (1);
	}
	if (cont != 0)
	{
		if (*input)
			free(*input);
		write(2, err, ft_strlen(err));
		shell->last_status = 2;
		return (1);
	}
	return (0);
}

static int	handle_input(t_shell *shell, char *input)
{
	if (check_continuation(shell, &input))
		return (1);
	consume_sigint(shell);
	return (process_line(input, shell) == 2);
}

static int	handle_null_input(t_shell *shell)
{
	if (g_signal == SIGINT)
		consume_sigint(shell);
	handle_eof(shell->last_status);
	return (1);
}

int	prompt_loop(t_shell *shell)
{
	char	*input;
	int		ret;

	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			ret = handle_null_input(shell);
			if (ret == -1)
				continue ;
			break ;
		}
		if (handle_sigint_empty(shell, input))
			continue ;
		if (handle_input(shell, input) || shell->should_exit)
			break ;
	}
	if (shell->should_exit)
		return (shell->exit_code);
	return (shell->last_status);
}
