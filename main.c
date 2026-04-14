/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/14 16:49:26 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
			if (current_argc == 0)
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
		input = readline("minishell$ ");
		if (!input)
		{
			handle_eof(shell.last_status);
			break ;
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

// 1. Mandatory builtins not implemented yet
// cd, export, unset are placeholders returning "not implemented yet" in builtin_state.c:15, builtin_state.c:22, builtin_state.c:29
// 2. Parser/AST contract incomplete for redirections
// t_cmd.redirs exists in minishell.h:59, but process_input currently only builds argv/pipes in main.c:84 and does not build redirection lists.
// So <, >, >>, << are tokenized but not fully parsed/executed as required.
// 3. Signals behavior likely incomplete
// One global signal variable is good (main.c:15), but full interactive behavior (Ctrl-C, Ctrl-\, prompt redisplay) is not shown as complete yet.
// 4. Bash parity differences observed
// pwd extra differs from bash behavior in your current implementation (builtin_io.c:44).

// Plan to fix redirections end-to-end:

// 1. Define parsing contract clearly
// t_cmd.argv = only command + args
// t_cmd.redirs = linked list of redirections in encounter order
// each redir node:
// type: R_IN, R_OUT, R_APPEND, R_HEREDOC
// target: filename or heredoc delimiter
// 2. Add parser helpers (in main.c or new parser file)
// new_redir(type, target)
// add_redir(cmd, redir)
// map_token_to_redir_type(token_type)
// parse_redirection(it, current_cmd):
// expects redir token then filename token
// on error prints syntax error and fails cleanly
// 3. Update process_input token loop
// current behavior:
// T_COMMAND/T_ARG -> append to argv
// T_PIPE -> finalize current command, allocate next
// new behavior add:
// T_REDIR_IN/T_REDIR_OUT/T_APPEND/T_HEREDOC:
// consume next token as target
// create redir node
// append to current->redirs
// do not add redirection tokens to argv
// 4. Syntax checks to enforce
// redirection without target: cat >
// pipe with empty side: | ls, ls |
// two operators in invalid sequence: cat > | wc
// on syntax failure:
// free tokens
// free partially built cmd/redirs
// return empty cmd + status 2 style
// 5. Memory management updates
// extend free_cmd_list to free each command’s redirs list (target + node)
// this is required before testing heavily
// 6. Execution integration
// before exec in child:
// apply redirections from cmd->redirs in order
// last input redirection wins for stdin
// last output/append wins for stdout
// for pipeline:
// set pipe fds first, then apply redirections (redir should override pipe end if both exist)
// 7. Heredoc (<<) strategy
// parser stores delimiter in redir node
// execution phase handles reading until delimiter and wiring input fd
// can first implement parse + storage, then execute part next
// 8. Validation sequence
// cat < infile
// echo hi > out
// echo hi >> out
// cat < infile | grep x > out
// error cases: cat >, | ls, cat <<
