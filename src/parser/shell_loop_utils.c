/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 22:10:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/05 03:10:06 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	scan_pipe_char(char c, char *quote, char *last, int *has_non_pipe)
{
	if (!*quote && (c == '\'' || c == '"'))
		*quote = c;
	else if (*quote && c == *quote)
		*quote = 0;
	if (!*quote && c != ' ' && c != '\t' && c != '|')
		*has_non_pipe = 1;
	if (c != ' ' && c != '\t')
		*last = c;
}

int	needs_pipe_continuation(const char *s)
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

char	*append_line(char *input, char *next)
{
	char	*tmp;
	char	*joined;

	tmp = ft_strjoin(input, "\n");
	free(input);
	if (!tmp)
	{
		free(next);
		return (NULL);
	}
	joined = ft_strjoin(tmp, next);
	free(tmp);
	free(next);
	return (joined);
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
