/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/21 16:11:17 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redir_token(t_token_type type)
{
	if (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC)
		return (1);
	return (0);
}

static int	syntax_token_error(char *token)
{
	if (!token)
		token = "newline";
	error_msg(2, "syntax error near unexpected token", NULL, token);
	return (0);
}

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

static int	add_redir(t_cmd *cmd, t_redir_type type, const char *target)
{
	t_redir	*new_redir;
	t_redir	*last;

	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return (0);
	new_redir->type = type;
	new_redir->target = ft_strdup(target);
	new_redir->heredoc_fd = -1;
	new_redir->next = NULL;
	if (!new_redir->target)
	{
		free(new_redir);
		return (0);
	}
	if (!cmd->redirs)
	{
		cmd->redirs = new_redir;
		return (1);
	}
	last = cmd->redirs;
	while (last->next)
		last = last->next;
	last->next = new_redir;
	return (1);
}

int	parse_redirection(t_cmd *cmd, t_token **it)
{
	t_token	*target;
	char	*tok;

	target = (*it)->next;
	if (!target || target->type == T_PIPE || is_redir_token(target->type))
	{
		tok = NULL;
		if (target)
			tok = target->value;
		syntax_token_error(tok);
		return (0);
	}
	if (!add_redir(cmd, map_redir_type((*it)->type), target->value))
		return (0);
	*it = target;
	return (1);
}
