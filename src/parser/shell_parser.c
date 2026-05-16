/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:00:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_token_error(char **argv, int status)
{
	if (argv)
		dfree(argv);
	if (status == -1)
		write(2, "./minishell: malloc failure\n", 28);
	if (status == 2)
		syntax_token_error("|");
	return (0);
}

static int	build_tokens(t_token *it, t_cmd *cmd, char **argv, int *argc)
{
	t_cmd	*cur;
	int		status;

	cur = cmd;
	while (it)
	{
		status = handle_token(&it, &cur, &argv, argc);
		if (status == -1 || status == -2 || status == 2)
			return (handle_token_error(argv, status));
	}
	cur->argv = argv;
	if (!cur->argv && !cur->redirs && cur != cmd)
	{
		syntax_token_error("newline");
		return (0);
	}
	return (1);
}

static t_cmd	*parse_tokens(t_token **tokens, t_shell *shell)
{
	t_cmd	*cmd;
	char	**argv;
	int		argc;

	expand_variables(tokens, shell);
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	init_cmd(cmd);
	argv = NULL;
	argc = 0;
	if (!build_tokens(*tokens, cmd, argv, &argc))
	{
		free_cmd_list(cmd);
		return (NULL);
	}
	return (cmd);
}

t_cmd	*process_input(char *input, t_shell *shell)
{
	t_token	*tokens;
	t_cmd	*cmd;

	if (!input)
		return (NULL);
	tokens = lexer(input);
	if (!tokens)
		return (NULL);
	cmd = parse_tokens(&tokens, shell);
	free_tokens(tokens);
	return (cmd);
}
