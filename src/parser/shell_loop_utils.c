/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 22:10:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/05 05:23:24 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	scan_pipe_char(char c, char *quote, char *last, int *has_non_pipe)
{
	if (!*quote && (c == '\'' || c == '"'))
		*quote = c;
	else if (*quote && c == *quote)
		*quote = 0;
	if (!*quote && c != ' ' && c != '\t' && c != '|' && c != '<' && c != '>')
		*has_non_pipe = 1;
	if (c != ' ' && c != '\t')
		*last = c;
}

static int	needs_pipe_continuation(const char *s)
{
	int		i;
	int		has_non_pipe;
	char	quote;
	char	last;

	if (!s)
		return (0);
	i = 0;
	has_non_pipe = 0;
	quote = 0;
	last = 0;
	while (s[i])
	{
		scan_pipe_char(s[i], &quote, &last, &has_non_pipe);
		i++;
	}
	if (quote)
		return (0);
	return (last == '|' && has_non_pipe);
}

int	read_pipe_continuation(char **input)
{
	char	*next;

	while (needs_pipe_continuation(*input))
	{
		next = readline("> ");
		if (!next)
			return (1);
		*input = append_line(*input, next);
		if (!*input)
			return (-1);
	}
	return (0);
}

static int	has_unclosed_quote(const char *s)
{
	char	quote;
	int		i;

	if (!s)
		return (0);
	quote = 0;
	i = 0;
	while (s[i])
	{
		if (!quote && (s[i] == '\'' || s[i] == '"'))
			quote = s[i];
		else if (quote && s[i] == quote)
			quote = 0;
		i++;
	}
	return (quote != 0);
}

int	read_quote_continuation(char **input)
{
	char	*next;

	while (has_unclosed_quote(*input))
	{
		next = readline("> ");
		if (!next)
			return (1);
		*input = append_line(*input, next);
		if (!*input)
			return (-1);
	}
	return (0);
}
