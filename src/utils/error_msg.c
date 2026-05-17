/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_msg.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:43 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/17 18:47:08 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// static void	write_sanitized(const char *s)
// {
// 	size_t	i;
// 	char	c;

// 	i = 0;
// 	while (s[i])
// 	{
// 		if (s[i] == '\n')
// 			c = ' ';
// 		else
// 			c = s[i];
// 		write(2, &c, 1);
// 		i++;
// 	}
// }

static int	append_join(char **dst, const char *suffix)
{
	char	*tmp;

	if (!*dst || !suffix)
		return (0);
	tmp = ft_strjoin(*dst, suffix);
	free(*dst);
	*dst = tmp;
	return (tmp != NULL);
}

int	error_msg(int status, char *cmd, char *arg, char *msg)
{
	char	*full;

	full = ft_strdup("./minishell: ");
	if (full && cmd)
	{
		append_join(&full, cmd);
		append_join(&full, ": ");
	}
	if (full && arg)
	{
		append_join(&full, arg);
		append_join(&full, ": ");
	}
	if (full && msg)
		append_join(&full, msg);
	if (full)
	{
		append_join(&full, "\n");
		if (full)
		{
			write(2, full, ft_strlen(full));
			free(full);
		}
	}
	return (status);
}

int	syntax_token_error(const char *token)
{
	write(2, "./minishell: syntax error near unexpected token `", 49);
	write(2, token, ft_strlen(token));
	write(2, "'\n", 2);
	return (2);
}
