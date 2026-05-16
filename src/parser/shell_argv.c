/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_argv.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/14 00:00:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/14 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_cmd_args(t_token *it)
{
	int	count;

	count = 0;
	while (it && it->type != T_PIPE)
	{
		if (it->type == T_ARG)
		{
			if (!it->remove_if_empty || (it->value && it->value[0]))
				count++;
		}
		else if (is_redir_token(it->type) && it->next)
			it = it->next;
		it = it->next;
	}
	return (count);
}

static int	fill_argv(t_token *it, char **argv, int count)
{
	int	i;

	i = 0;
	while (it && it->type != T_PIPE && i < count)
	{
		if (it->type == T_ARG && (!it->remove_if_empty
				|| (it->value && it->value[0])))
		{
			argv[i] = ft_strdup(it->value);
			if (!argv[i])
			{
				while (i-- > 0)
					free(argv[i]);
				return (0);
			}
			i++;
		}
		else if (is_redir_token(it->type) && it->next)
			it = it->next;
		it = it->next;
	}
	argv[i] = NULL;
	return (1);
}

char	**alloc_argv(t_token *it, int *count_out)
{
	int		count;
	char	**argv;

	count = count_cmd_args(it);
	if (count_out)
		*count_out = count;
	if (count == 0)
		return (NULL);
	argv = malloc(sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);
	if (!fill_argv(it, argv, count))
	{
		free(argv);
		return (NULL);
	}
	return (argv);
}
