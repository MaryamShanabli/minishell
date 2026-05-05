/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_builder.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 16:34:37 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/04 16:21:57 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	operator_len(char *line, int i)
{
	if (line[i] == '>' && line[i + 1] == '>')
		return (2);
	if (line[i] == '<' && line[i + 1] == '<')
		return (2);
	return (1);
}

static t_token	*new_token(char *value, t_token_type type)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->value = ft_strdup(value);
	if (!tok->value)
	{
		free(tok);
		return (NULL);
	}
	tok->type = type;
	tok->remove_if_empty = 0;
	tok->next = NULL;
	return (tok);
}

static void	add_token(t_token **list, t_token *new)
{
	t_token	*tmp;

	if (!new)
		return ;
	if (!*list)
	{
		*list = new;
		return ;
	}
	tmp = *list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
}

int	add_operator_token(char *line, int *i, t_token **tokens,
		int *first_word)
{
	int		len;
	t_token	*tok;

	len = operator_len(line, *i);
	if (line[*i] == '|')
	{
		tok = new_token("|", T_PIPE);
		*first_word = 1;
	}
	else if (line[*i] == '>' && len == 2)
		tok = new_token(">>", T_APPEND);
	else if (line[*i] == '>')
		tok = new_token(">", T_REDIR_OUT);
	else if (line[*i] == '<' && len == 2)
		tok = new_token("<<", T_HEREDOC);
	else
		tok = new_token("<", T_REDIR_IN);
	if (!tok)
		return (0);
	add_token(tokens, tok);
	*i += len;
	return (1);
}

int	add_word_token(char *line, int *i, t_token **tokens,
		int *first_word)
{
	char			*word;
	t_token_type	type;
	t_token			*tok;

	word = read_word(line, i);
	if (!word || !word[0])
	{
		if (word)
			free(word);
		return (0);
	}
	if (*first_word)
		type = T_COMMAND;
	else
		type = T_ARG;
	tok = new_token(word, type);
	free(word);
	if (!tok)
		return (0);
	add_token(tokens, tok);
	*first_word = 0;
	return (1);
}
