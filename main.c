/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/05 17:12:59 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

static void	handle_eof(int status)
{
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	(void)status;
}

static void	free_tokens(t_token *tokens)
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

static void	init_cmd(t_cmd *cmd)
{
	cmd->argv = NULL;
	cmd->pid = 0;
	cmd->next = NULL;
}

static void	free_cmd_list(t_cmd *cmd)
{
	t_cmd	*next;
	t_cmd	*node;

	if (!cmd)
		return ;
	if (cmd->argv)
		dfree(cmd->argv);
	node = cmd->next;
	while (node)
	{
		next = node->next;
		if (node->argv)
			dfree(node->argv);
		free(node);
		node = next;
	}
	cmd->next = NULL;
}

static int	append_arg(char ***argv, int *argc, const char *value)
{
	char	**new_argv;

	new_argv = realloc(*argv, sizeof(char *) * (*argc + 2));
	if (!new_argv)
		return (0);
	*argv = new_argv;
	(*argv)[*argc] = ft_strdup(value);
	if (!(*argv)[*argc])
	{
		dfree(*argv);
		*argv = NULL;
		return (0);
	}
	(*argc)++;
	(*argv)[*argc] = NULL;
	return (1);
}

static int	syntax_pipe_error(void)
{
	error_msg(2, "syntax error near unexpected token", "`|'");
	return (0);
}

t_cmd	process_input(char *input, int last_status)
{
	t_cmd	cmd;
	t_token	*tokens;
	t_token	*it;
	t_cmd	*current;
	char	**current_argv;
	int		current_argc;

	init_cmd(&cmd);
	if (!input)
		return (cmd);
	tokens = lexer(input);
	if (!tokens)
		return (cmd);
	expand_variables(tokens, last_status);
	current = &cmd;
	current_argv = NULL;
	current_argc = 0;
	it = tokens;
	while (it)
	{
		if (it->type == T_COMMAND || it->type == T_ARG)
		{
			if (!append_arg(&current_argv, &current_argc, it->value))
				return (free_tokens(tokens), free_cmd_list(&cmd), cmd);
		}
		else if (it->type == T_PIPE)
		{
			if (current_argc == 0)
				return (free_tokens(tokens), free_cmd_list(&cmd), syntax_pipe_error(), cmd);
			current->argv = current_argv;
			current_argv = NULL;
			current_argc = 0;
			current->next = malloc(sizeof(t_cmd));
			if (!current->next)
				return (free_tokens(tokens), free_cmd_list(&cmd), cmd);
			current = current->next;
			init_cmd(current);
		}
		it = it->next;
	}
	if (current_argc == 0 && cmd.next)
		return (free_tokens(tokens), free_cmd_list(&cmd), syntax_pipe_error(), cmd);
	current->argv = current_argv;
	free_tokens(tokens);
	return (cmd);
}

int	main(int ac, char **av, char **envp)
{
	char	*input;
	t_cmd	cmd;
	int		status;

	status = 0;
	if (ac > 1)
	{
		input = av[1];
		cmd = process_input(input, status);
		status = execute(&cmd, envp, status);
		free_cmd_list(&cmd);
		return (status);
	}
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			handle_eof(status);
			break ;
		}
		add_history(input);
		cmd = process_input(input, status);
		status = execute(&cmd, envp, status);
		if (cmd.argv && !cmd.next && !ft_strcmp(cmd.argv[0], "exit"))
		{
			free_cmd_list(&cmd);
			free(input);
			break ;
		}
		free_cmd_list(&cmd);
		free(input);
	}
	exit(status);
}
