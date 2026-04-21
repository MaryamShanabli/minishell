/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_lexer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 16:27:29 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/21 15:10:27 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_word_break(char c)
{
	return (c == ' ' || c == '|' || c == '>' || c == '<');
}

static void	advance_word(char *line, int *i, char *quote)
{
	if (*quote)
	{
		if (line[*i] == *quote)
			*quote = 0;
		(*i)++;
	}
	else if (line[*i] == '\'' || line[*i] == '"')
	{
		*quote = line[*i];
		(*i)++;
	}
	else
		(*i)++;
}

char	*read_word(char *line, int *i)
{
	int		start;
	int		len;
	char	quote;
	char	*word;

	start = *i;
	quote = 0;
	while (line[*i])
	{
		if (!quote && is_word_break(line[*i]))
			break ;
		advance_word(line, i, &quote);
	}
	len = *i - start;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	ft_strncpy(word, line + start, len);
	word[len] = '\0';
	return (word);
}

t_token	*lexer(char *line)
{
	int		i;
	t_token	*tokens;
	int		first_word;

	i = 0;
	tokens = NULL;
	first_word = 1;
	while (line[i])
	{
		while (line[i] == ' ')
			i++;
		if (!line[i])
			break ;
		if (line[i] == '|' || line[i] == '>' || line[i] == '<')
			add_operator_token(line, &i, &tokens, &first_word);
		else
			add_word_token(line, &i, &tokens, &first_word);
	}
	return (tokens);
}

void	print_tokens(t_token *list)
{
	while (list)
	{
		printf("TOKEN: %-10s TYPE: %d\n", list->value, list->type);
		list = list->next;
	}
}
