/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_parse_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:20:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/21 16:36:20 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_new_argv(char **new_argv, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		new_argv[i] = NULL;
		i++;
	}
}

static void	copy_old_argv(char ***argv, char **new_argv, int argc)
{
	int	i;

	i = 0;
	while (i < argc)
	{
		new_argv[i] = (*argv)[i];
		i++;
	}
	free(*argv);
}

static int	append_arg(char ***argv, int *argc, const char *value)
{
	char	**new_argv;

	new_argv = malloc(sizeof(char *) * (*argc + 3));
	if (!new_argv)
		return (0);
	init_new_argv(new_argv, *argc + 3);
	if (*argv)
		copy_old_argv(argv, new_argv, *argc);
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

int	handle_token(t_token **it, t_cmd **current, char ***argv, int *argc)
{
	int	status;

	if ((*it)->type == T_COMMAND || (*it)->type == T_ARG)
	{
		if ((*it)->remove_if_empty && (!(*it)->value || !(*it)->value[0]))
		{
			*it = (*it)->next;
			return (1);
		}
		if (!append_arg(argv, argc, (*it)->value))
			return (-1);
	}
	else if (is_redir_token((*it)->type))
	{
		if (!parse_redirection(*current, it))
			return (-2);
	}
	else if ((*it)->type == T_PIPE)
	{
		status = handle_pipe_token(current, argv, argc);
		return (status);
	}
	*it = (*it)->next;
	return (1);
}
