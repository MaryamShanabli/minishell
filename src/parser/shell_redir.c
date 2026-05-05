/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/05 03:11:44 by mshanabl         ###   ########.fr       */
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

int	parse_redirection(t_cmd *cmd, t_token **it)
{
	t_token	*target;
	char	*tok;
	int		status;

	target = (*it)->next;
	if (!target || target->type == T_PIPE || is_redir_token(target->type))
	{
		tok = NULL;
		if (target)
			tok = target->value;
		syntax_token_error(tok);
		return (0);
	}
	status = add_redirection(cmd, (*it)->type,
			target->value, target->remove_if_empty);
	if (!status)
		return (0);
	*it = target;
	return (1);
}
