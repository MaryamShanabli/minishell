/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 00:00:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_ifs(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static t_token	*make_word_token(const char *start, size_t len)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->value = malloc(len + 1);
	if (!tok->value)
	{
		free(tok);
		return (NULL);
	}
	ft_strncpy(tok->value, start, len);
	tok->value[len] = '\0';
	tok->type = T_ARG;
	tok->remove_if_empty = 0;
	tok->next = NULL;
	return (tok);
}

static t_token	*append_word(t_token **head, t_token **tail, t_token *tok)
{
	if (!*head)
		*head = tok;
	else
		(*tail)->next = tok;
	*tail = tok;
	return (tok);
}

static t_token	*split_value(const char *val, t_token *rest)
{
	t_token	*head;
	t_token	*tail;
	t_token	*tok;
	size_t	i;
	size_t	start;

	head = NULL;
	tail = NULL;
	i = 0;
	while (val[i])
	{
		while (val[i] && is_ifs(val[i]))
			i++;
		if (!val[i])
			break ;
		start = i;
		while (val[i] && !is_ifs(val[i]))
			i++;
		tok = make_word_token(val + start, i - start);
		if (!tok || !append_word(&head, &tail, tok))
			return (NULL);
	}
	if (tail)
		tail->next = rest;
	return (head);
}

t_token	*word_split_token(t_token *tok, t_token *next)
{
	t_token	*result;

	if (!tok->value || !tok->value[0])
	{
		tok->remove_if_empty = 1;
		tok->next = next;
		return (tok);
	}
	result = split_value(tok->value, next);
	free(tok->value);
	free(tok);
	if (!result)
		return (next);
	return (result);
}
