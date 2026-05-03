/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_msg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:45 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 18:21:40 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	error_msg(int status, const char *cmd, const char *arg, const char *msg)
{
	write(2, "./minishell: ", 13);
	write(2, cmd, ft_strlen(cmd));
	write(2, ": ", 2);
	if (arg)
	{
		write(2, arg, ft_strlen(arg));
		write(2, ": ", 2);
	}
	write(2, msg, ft_strlen(msg));
	write(2, "\n", 1);
	if (status == 0)
		return (0);
	return (status);
}

int	syntax_pipe_error(void)
{
	error_msg(2, "syntax error near unexpected token", NULL, "`|'");
	return (0);
}

void	exec_dot_error(void)
{
	write(2, "./minishell: .: filename argument required\n", 43);
	write(2, "./minishell: .: usage: . filename [arguments]\n", 46);
	exit(2);
}
