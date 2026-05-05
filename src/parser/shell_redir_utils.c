/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_redir_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/04 22:20:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/05 03:00:06 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_redir_type	map_redir_type(t_token_type type)
{
	if (type == T_REDIR_IN)
		return (R_IN);
	if (type == T_REDIR_OUT)
		return (R_OUT);
	if (type == T_APPEND)
		return (R_APPEND);
	return (R_HEREDOC);
}

static t_redir	*new_redir(t_redir_type type, const char *target,
		int expand, int was_unquoted_var)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->target = ft_strdup(target);
	redir->heredoc_fd = -1;
	redir->heredoc_expand = expand;
	redir->target_was_unquoted_var = was_unquoted_var;
	redir->next = NULL;
	if (!redir->target)
	{
		free(redir);
		return (NULL);
	}
	return (redir);
}

static int	append_redir(t_cmd *cmd, t_redir *redir)
{
	t_redir	*last;

	if (!cmd->redirs)
	{
		cmd->redirs = redir;
		return (1);
	}
	last = cmd->redirs;
	while (last->next)
		last = last->next;
	last->next = redir;
	return (1);
}

static char	*unquote_heredoc_delim(const char *in, int *expand)
{
	size_t	i;
	size_t	j;
	size_t	len;
	char	*out;

	i = 0;
	j = 0;
	len = ft_strlen(in);
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	*expand = 1;
	while (in[i])
	{
		if (in[i] == '\'' || in[i] == '"')
		{
			*expand = 0;
			i++;
		}
		else
			out[j++] = in[i++];
	}
	out[j] = '\0';
	return (out);
}

int	add_redirection(t_cmd *cmd, t_token_type type,
		const char *target, int was_unquoted_var)
{
	t_redir	*redir;
	char	*delim;
	int		expand;

	if (type == T_HEREDOC)
	{
		delim = unquote_heredoc_delim(target, &expand);
		if (!delim)
			return (0);
		redir = new_redir(map_redir_type(type), delim, expand, 0);
		free(delim);
	}
	else
		redir = new_redir(map_redir_type(type), target, 1, was_unquoted_var);
	if (!redir)
		return (0);
	return (append_redir(cmd, redir));
}
