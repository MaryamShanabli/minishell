/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_msg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:43 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	write_sanitized(const char *s)
{
	size_t	i;
	char	c;

	i = 0;
	while (s[i])
	{
		if (s[i] == '\n')
			c = ' ';
		else
			c = s[i];
		write(2, &c, 1);
		i++;
	}
}

int	error_msg(int status, const char *cmd, const char *arg, const char *msg)
{
	write(2, "./minishell: ", 13);
	if (cmd)
	{
		write_sanitized(cmd);
		write(2, ": ", 2);
	}
	if (arg)
	{
		write_sanitized(arg);
		write(2, ": ", 2);
	}
	if (msg)
		write(2, msg, ft_strlen(msg));
	write(2, "\n", 1);
	return (status);
}

int	syntax_token_error(const char *token)
{
	write(2, "./minishell: syntax error near unexpected token `", 49);
	write(2, token, ft_strlen(token));
	write(2, "'\n", 2);
	return (2);
}
