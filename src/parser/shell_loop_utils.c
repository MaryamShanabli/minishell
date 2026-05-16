/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 22:10:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/16 12:52:56 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	scan_pipe_char(char c, t_pipe_scan *scan)
{
	if (!scan->quote && (c == '\'' || c == '"'))
		scan->quote = c;
	else if (scan->quote && c == scan->quote)
		scan->quote = 0;
	if (!scan->quote && c != ' ' && c != '\t' && c != '\n'
		&& c != '|' && c != '<' && c != '>')
		scan->has_non_pipe = 1;
	if (!scan->quote && c != ' ' && c != '\t' && c != '\n')
	{
		scan->prev = scan->last;
		scan->last = c;
	}
}

static int	needs_pipe_continuation(const char *s)
{
	int			i;
	t_pipe_scan	scan;

	if (!s)
		return (0);
	i = 0;
	scan.quote = 0;
	scan.prev = 0;
	scan.last = 0;
	scan.has_non_pipe = 0;
	while (s[i])
	{
		scan_pipe_char(s[i], &scan);
		i++;
	}
	if (scan.quote)
		return (0);
	if (scan.last != '|' || !scan.has_non_pipe)
		return (0);
	if (scan.prev == '|')
		return (0);
	return (1);
}

int	read_pipe_continuation(char **input)
{
	char	*next;

	while (needs_pipe_continuation(*input))
	{
		next = readline("> ");
		if (!next)
		{
			if (g_signal == SIGINT)
				return (-1);
			return (1);
		}
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
		{
			if (g_signal == SIGINT)
				return (-1);
			return (1);
		}
		*input = append_line(*input, next);
		if (!*input)
			return (-1);
	}
	return (0);
}
