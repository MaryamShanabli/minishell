/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_var.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:52 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 20:21:08 by mshanabl         ###   ########.fr       */
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

static char	*trim_ifs_edges(const char *s)
{
	size_t	start;
	size_t	end;
	char	*out;
	char	*empty;

	if (!s)
	{
		empty = ft_strdup("");
		return (empty);
	}
	start = 0;
	while (s[start] == ' ' || s[start] == '\t' || s[start] == '\n')
		start++;
	end = ft_strlen(s);
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'
			|| s[end - 1] == '\n'))
		end--;
	out = malloc(end - start + 1);
	if (!out)
		return (NULL);
	ft_strncpy(out, s + start, end - start);
	out[end - start] = '\0';
	return (out);
}

static int	expand_token(t_token *tok, t_shell *shell)
{
	char	*new_val;
	char	*trimmed;
	int		quoted;

	quoted = has_quotes(tok->value);
	new_val = expand_one(tok->value, shell);
	if (!new_val)
		return (0);
	if (!quoted)
	{
		trimmed = trim_ifs_edges(new_val);
		free(new_val);
		if (!trimmed)
			return (0);
		new_val = trimmed;
		tok->remove_if_empty = (new_val[0] == '\0');
	}
	else
		tok->remove_if_empty = 0;
	free(tok->value);
	tok->value = new_val;
	return (1);
}

void	expand_variables(t_token *tokens, t_shell *shell)
{
	int	skip_next;

	skip_next = 0;
	while (tokens)
	{
		if (tokens->type == T_HEREDOC)
		{
			skip_next = 1;
			tokens = tokens->next;
			continue ;
		}
		if (skip_next)
		{
			skip_next = 0;
			tokens = tokens->next;
			continue ;
		}
		if (tokens->type == T_COMMAND || tokens->type == T_ARG)
		{
			if (!expand_token(tokens, shell))
				return ;
		}
		tokens = tokens->next;
	}
}
