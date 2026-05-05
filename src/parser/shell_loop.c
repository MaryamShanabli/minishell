/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/05 05:24:37 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	interactive_loop(t_shell *shell)
{
	char	*msg;
	size_t	msg_len;

	msg = "./minishell: syntax error: unexpected end of file\n";
	msg_len = ft_strlen(msg);
	prompt_loop(shell, msg, msg_len);
	return (shell->last_status);
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
