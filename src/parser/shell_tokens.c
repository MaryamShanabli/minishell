/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_pipe_token(t_cmd **current, t_token **it)
{
	t_cmd	*new_cmd;

	if (!(*current)->argv && !(*current)->redirs)
		return (2);
	new_cmd = malloc(sizeof(t_cmd));
	if (!new_cmd)
		return (-1);
	(*current)->next = new_cmd;
	*current = new_cmd;
	init_cmd(*current);
	*it = (*it)->next;
	return (1);
}

static int	handle_arg_token(t_token **it, char ***argv, int *argc)
{
	if (!*argv)
	{
		*argv = alloc_argv(*it, argc);
		while (*it && (*it)->type == T_ARG)
			*it = (*it)->next;
		if (!*argv && *argc > 0)
			return (-1);
		return (1);
	}
	*it = (*it)->next;
	return (1);
}

int	handle_token(t_token **it, t_cmd **current, char ***argv, int *argc)
{
	if ((*it)->type == T_ARG)
		return (handle_arg_token(it, argv, argc));
	if (is_redir_token((*it)->type))
	{
		if (!parse_redirection(*current, it))
			return (-2);
		return (1);
	}
	if ((*it)->type == T_PIPE)
	{
		(*current)->argv = *argv;
		*argv = NULL;
		*argc = 0;
		return (handle_pipe_token(current, it));
	}
	*it = (*it)->next;
	return (1);
}
