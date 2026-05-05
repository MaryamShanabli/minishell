/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/05/04 14:23:04 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**realloc_argv(char **old, int argc)
{
	char	**new_argv;
	int		i;

	new_argv = malloc(sizeof(char *) * (argc + 3));
	if (!new_argv)
		return (NULL);
	i = 0;
	while (i < argc + 3)
		new_argv[i++] = NULL;
	if (old)
	{
		i = 0;
		while (i < argc)
		{
			new_argv[i] = old[i];
			i++;
		}
		free(old);
	}
	return (new_argv);
}

static int	append_arg(char ***argv, int *argc, const char *value)
{
	char	**new_argv;

	new_argv = realloc_argv(*argv, *argc);
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

static int	handle_pipe_token(t_cmd **current, char ***argv, int *argc)
{
	t_cmd	*new_cmd;

	if (*argc == 0 && (*current)->redirs == NULL)
		return (2);
	(*current)->argv = *argv;
	*argv = NULL;
	*argc = 0;
	new_cmd = malloc(sizeof(t_cmd));
	if (!new_cmd)
		return (-1);
	(*current)->next = new_cmd;
	*current = (*current)->next;
	init_cmd(*current);
	return (1);
}

static int	handle_word_token(t_token **it, char ***argv, int *argc)
{
	if ((*it)->remove_if_empty && (!(*it)->value || !(*it)->value[0]))
	{
		*it = (*it)->next;
		return (1);
	}
	if (!append_arg(argv, argc, (*it)->value))
		return (-1);
	*it = (*it)->next;
	return (1);
}

int	handle_token(t_token **it, t_cmd **current, char ***argv, int *argc)
{
	int	status;

	if ((*it)->type == T_COMMAND || (*it)->type == T_ARG)
	{
		status = handle_word_token(it, argv, argc);
		return (status);
	}
	else if (is_redir_token((*it)->type))
	{
		if (!parse_redirection(*current, it))
			return (-2);
	}
	else if ((*it)->type == T_PIPE)
	{
		status = handle_pipe_token(current, argv, argc);
		if (status > 0)
			*it = (*it)->next;
		return (status);
	}
	*it = (*it)->next;
	return (1);
}
