/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:00:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/03 18:18:10 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	transfer_redirs(t_cmd *curr, t_cmd *next)
{
	if (next->redirs && !curr->redirs)
		curr->redirs = next->redirs;
	else if (next->redirs && next->next && !next->next->redirs)
		next->next->redirs = next->redirs;
	else if (next->redirs)
	{
		free_redirs(next->redirs);
		next->redirs = NULL;
	}
	next->redirs = NULL;
}

static void	unlink_empty_commands(t_cmd *first)
{
	t_cmd	*curr;
	t_cmd	*next;

	if (!first || !first->next)
		return ;
	curr = first;
	while (curr && curr->next)
	{
		next = curr->next;
		if (!next->argv || !next->argv[0])
		{
			transfer_redirs(curr, next);
			curr->next = next->next;
			free(next);
			next = curr->next;
		}
		else
			curr = next;
	}
}

static int	handle_token_error(char **argv, int status)
{
	if (argv)
		dfree(argv);
	if (status == 2)
		syntax_pipe_error();
	return (0);
}

static int	build_command_from_tokens(t_token *tokens, t_cmd *cmd,
	char **argv, int *argc)
{
	t_token	*it;
	t_cmd	*current;
	int		status;

	current = cmd;
	it = tokens;
	while (it)
	{
		status = handle_token(&it, &current, &argv, argc);
		if (status <= 0 || status == 2)
			return (handle_token_error(argv, status));
	}
	if (*argc == 0 && cmd->next)
	{
		if (argv)
			dfree(argv);
		syntax_pipe_error();
		return (0);
	}
	current->argv = argv;
	unlink_empty_commands(cmd);
	return (1);
}

t_cmd	process_input(char *input, t_shell *shell)
{
	t_cmd	cmd;
	t_token	*tokens;
	char	**current_argv;
	int		current_argc;

	init_cmd(&cmd);
	if (!input)
		return (cmd);
	tokens = lexer(input);
	if (!tokens)
		return (cmd);
	expand_variables(tokens, shell);
	current_argv = NULL;
	current_argc = 0;
	if (!build_command_from_tokens(tokens, &cmd, current_argv, &current_argc))
	{
		free_tokens(tokens);
		free_cmd_list(&cmd);
		init_cmd(&cmd);
		cmd.pid = -2;
		return (cmd);
	}
	free_tokens(tokens);
	return (cmd);
}
