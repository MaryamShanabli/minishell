/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_memory.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/03 17:50:20 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_tokens(t_token *tokens)
{
	t_token	*next;

	while (tokens)
	{
		next = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = next;
	}
}

void	free_redirs(t_redir *redirs)
{
	t_redir	*next;

	while (redirs)
	{
		next = redirs->next;
		free(redirs->target);
		free(redirs);
		redirs = next;
	}
}

void	free_cmd_list(t_cmd *cmd)
{
	t_cmd	*next;
	t_cmd	*node;

	if (!cmd)
		return ;
	if (cmd->argv)
		dfree(cmd->argv);
	if (cmd->redirs)
		free_redirs(cmd->redirs);
	node = cmd->next;
	while (node)
	{
		next = node->next;
		if (node->argv)
			dfree(node->argv);
		if (node->redirs)
			free_redirs(node->redirs);
		free(node);
		node = next;
	}
	cmd->next = NULL;
}

void	handle_eof(int status)
{
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	(void)status;
}
