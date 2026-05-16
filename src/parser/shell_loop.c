/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/16 13:24:13 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	interactive_loop(t_shell *shell)
{
	int	status;

	status = prompt_loop(shell);
	return (status);
}

int	consume_sigint(t_shell *shell)
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

void	sigint_display_fix(void)
{
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

int	handle_sigint_empty(t_shell *shell, char *input)
{
	if (g_signal == SIGINT && input && !*input)
	{
		consume_sigint(shell);
		free(input);
		return (1);
	}
	return (0);
}
