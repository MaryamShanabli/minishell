/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/14 19:00:53 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Global signal variable - stores the last received signal number */
volatile sig_atomic_t	g_signal = 0;

static void	signal_handler(int signum)
{
	g_signal = signum;
	if (signum == SIGINT)
		write(1, "\n", 1);
}

static void	setup_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	rl_catch_signals = 0;
}

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

static void	free_redirs(t_redir *redirs)
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

static int	is_redir_token(t_token_type type)
{
	if (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC)
		return (1);
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
		return (free(new_redir), 0);
	if (!cmd->redirs)
		return (cmd->redirs = new_redir, 1);
	last = cmd->redirs;
	while (last->next)
		last = last->next;
	last->next = new_redir;
	return (1);
}

static void	init_cmd(t_cmd *cmd)
{
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->is_builtin = 0;

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
	{
		dfree(cmd->argv);
		cmd->argv = NULL;
	}
	if (cmd->redirs)
	{
		free_redirs(cmd->redirs);
		cmd->redirs = NULL;
	}
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

static int	append_arg(char ***argv, int *argc, const char *value)
{
	char	**new_argv;
	int		i;

	new_argv = malloc(sizeof(char *) * (*argc + 2));
	if (!new_argv)
		return (0);
	i = 0;
	while (i < *argc)
	{
		new_argv[i] = (*argv)[i];
		i++;
	}
	free(*argv);
	*argv = new_argv;
	(*argv)[*argc] = ft_strdup(value);
	if (!(*argv)[*argc])
		return (dfree(*argv), *argv = NULL, 0);
	(*argc)++;
	(*argv)[*argc] = NULL;
	return (1);
}

static int	syntax_pipe_error(void)
{
	error_msg(2, "syntax error near unexpected token", NULL, "`|'");
	return (0);
}

static int	syntax_token_error(char *token)
{
	if (!token)
		token = "newline";
	error_msg(2, "syntax error near unexpected token", NULL, token);
	return (0);
}

static int	parse_redirection(t_cmd *cmd, t_token **it)
{
	t_token	*target;

	target = (*it)->next;
	if (!target || target->type == T_PIPE || is_redir_token(target->type))
		return (syntax_token_error(target ? target->value : NULL));
	/* Plan point 2/3: parse and store command redirections in order. */
	if (!add_redir(cmd, map_redir_type((*it)->type), target->value))
		return (0);
	*it = target;
	return (1);
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
			/* Found empty command, move its redirs to prev and unlink */
			if (next->redirs && !curr->redirs)
				curr->redirs = next->redirs;
			else if (next->redirs)
			{
				/* Move redirs to next->next if it exists */
				if (next->next && !next->next->redirs)
					next->next->redirs = next->redirs;
			}
			curr->next = next->next;
			free(next);
			next = curr->next;
		}
		else
			curr = next;
	}
}

t_cmd	process_input(char *input, t_shell *shell)
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
	expand_variables(tokens, shell);
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
		else if (is_redir_token(it->type))
		{
			if (!parse_redirection(current, &it))
				return (free_tokens(tokens), free_cmd_list(&cmd), cmd.pid = -2, cmd);
		}
		else if (it->type == T_PIPE)
		{
			if (current_argc == 0 && current->redirs == NULL)
				return (free_tokens(tokens), free_cmd_list(&cmd),
					syntax_pipe_error(), cmd.pid = -2, cmd);
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
		return (free_tokens(tokens), free_cmd_list(&cmd),
			syntax_pipe_error(), cmd.pid = -2, cmd);
	current->argv = current_argv;
	unlink_empty_commands(&cmd);
	free_tokens(tokens);
	return (cmd);
}

int	main(int ac, char **av, char **envp)
{
	char	*input;
	t_cmd	cmd;
	t_shell	shell;

	shell.env = env_dup(envp);
	if (!shell.env)
		return (1);
	shell.last_status = 0;
	setup_signals();
	if (ac > 1)
	{
		input = av[1];
		cmd = process_input(input, &shell);
		if (cmd.pid == -2)
			shell.last_status = 2;
		else
			shell.last_status = execute(&cmd, &shell);
		free_cmd_list(&cmd);
		dfree(shell.env);
		return (shell.last_status);
	}
	while (1)
	{
		if (g_signal == SIGINT)
		{
			g_signal = 0;
			continue ;
		}
		input = readline("minishell$ ");
		if (!input)
		{
			if (g_signal == SIGINT)
			{
				g_signal = 0;
				continue ;
			}
			handle_eof(shell.last_status);
			break ;
		}
		if (g_signal == SIGINT)
		{
			g_signal = 0;
			free(input);
			continue ;
		}
		if (!*input)
		{
			free(input);
			continue ;
		}
		add_history(input);
		cmd = process_input(input, &shell);
		if (cmd.pid == -2)
			shell.last_status = 2;
		else
			shell.last_status = execute(&cmd, &shell);
		if (cmd.argv && !cmd.next && !ft_strcmp(cmd.argv[0], "exit"))
		{
			free_cmd_list(&cmd);
			free(input);
			break ;
		}
		free_cmd_list(&cmd);
		free(input);
	}
	dfree(shell.env);
	exit(shell.last_status);
}

