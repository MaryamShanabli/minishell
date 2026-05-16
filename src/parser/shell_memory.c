/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_memory.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/12 00:00:00 by mshanabl         ###   ########.fr       */
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
		if (redirs->heredoc_fd > 0)
			close(redirs->heredoc_fd);
		free(redirs->target);
		free(redirs);
		redirs = next;
	}
}

void	free_cmd_list(t_cmd *cmd)
{
	t_cmd	*next;

	while (cmd)
	{
		next = cmd->next;
		if (cmd->argv)
			dfree(cmd->argv);
		if (cmd->redirs)
			free_redirs(cmd->redirs);
		free(cmd);
		cmd = next;
	}
}

void	handle_eof(int status)
{
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	(void)status;
}

char	*append_line(char *input, char *next)
{
	char	*tmp;
	char	*joined;

	tmp = ft_strjoin(input, "\n");
	free(input);
	if (!tmp)
	{
		free(next);
		return (NULL);
	}
	joined = ft_strjoin(tmp, next);
	free(tmp);
	free(next);
	return (joined);
}
