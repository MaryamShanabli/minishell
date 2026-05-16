/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:52 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	has_quotes(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (s[i] == '\'' || s[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

static void	expand_in_quotes(t_token *tok, t_shell *shell)
{
	char	*v;

	v = expand_one(tok->value, shell);
	if (!v)
		return ;
	free(tok->value);
	tok->value = v;
	tok->remove_if_empty = 0;
}

static t_token	*expand_unquoted(t_token *tok, t_shell *shell)
{
	char	*v;
	t_token	*rest;

	rest = tok->next;
	v = expand_one(tok->value, shell);
	if (!v)
		return (tok);
	free(tok->value);
	tok->value = v;
	return (word_split_token(tok, rest));
}

static t_token	**process_arg(t_token **pp, t_shell *shell)
{
	t_token	*result;
	t_token	*cur;
	t_token	*end;

	if (has_quotes((*pp)->value))
	{
		expand_in_quotes(*pp, shell);
		return (&(*pp)->next);
	}
	end = (*pp)->next;
	result = expand_unquoted(*pp, shell);
	*pp = result;
	cur = result;
	while (cur && cur->next != end)
		cur = cur->next;
	if (cur)
		return (&cur->next);
	return (pp);
}

void	expand_variables(t_token **head, t_shell *shell)
{
	t_token	**pp;
	int		skip;

	pp = head;
	skip = 0;
	while (*pp)
	{
		if ((*pp)->type == T_HEREDOC || skip)
		{
			skip = ((*pp)->type == T_HEREDOC);
			pp = &(*pp)->next;
		}
		else if ((*pp)->type == T_ARG)
			pp = process_arg(pp, shell);
		else
			pp = &(*pp)->next;
	}
}
