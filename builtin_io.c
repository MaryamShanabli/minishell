/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_io.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:11 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/12 20:17:06 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->argv[1] && !ft_strcmp(cmd->argv[1], "-n"))
	{
		newline = 0;
		i++;
	}
	while (cmd->argv[i])
	{
		write(1, cmd->argv[i], ft_strlen(cmd->argv[i]));
		if (cmd->argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}

int	builtin_pwd(t_cmd *cmd)
{
	char	cwd[1024];

	if (cmd->argv[1])
		return (error_msg(1, "pwd", NULL, "too many arguments"));
	if (!getcwd(cwd, sizeof(cwd)))
		return (perror("pwd"), 1);
	write(1, cwd, ft_strlen(cwd));
	write(1, "\n", 1);
	return (0);
}

int	builtin_env(t_cmd *cmd)
{
	extern char	**environ;
	int			i;
	int			has_equal;
	int			j;

	if (cmd->argv[1])
		return (error_msg(1, "env", NULL, "no options or arguments supported"));
	i = 0;
	while (environ[i])
	{
		has_equal = 0;
		j = 0;
		while (environ[i][j])
		{
			if (environ[i][j] == '=')
			{
				has_equal = 1;
				break ;
			}
			j++;
		}
		if (has_equal)
		{
			write(1, environ[i], ft_strlen(environ[i]));
			write(1, "\n", 1);
		}
		i++;
	}
	return (0);
}
