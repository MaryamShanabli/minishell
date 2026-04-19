/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_io.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:25 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 16:08:52 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	has_n(const char *arg)
{
	int	j;

	if (!arg || arg[0] != '-')
		return (0);
	j = 1;
	if (arg[j] == '\0')
		return (0);
	while (arg[j] == 'n')
		j++;
	if (arg[j] != '\0')
		return (0);
	return (1);
}

int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (cmd->argv[i] && has_n(cmd->argv[i]))
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
	int		argc;
	int		err;

	argc = 0;
	while (cmd->argv && cmd->argv[argc])
		argc++;
	if (argc > 1)
	{
		err = error_msg(1, "pwd", NULL, "too many arguments");
		return (err);
	}
	if (!getcwd(cwd, sizeof(cwd)))
	{
		perror("pwd");
		return (1);
	}
	write(1, cwd, ft_strlen(cwd));
	write(1, "\n", 1);
	return (0);
}

static int	has_equal(const char *entry)
{
	int	j;

	j = 0;
	while (entry[j])
	{
		if (entry[j] == '=')
			return (1);
		j++;
	}
	return (0);
}

int	builtin_env(t_cmd *cmd, t_shell *shell)
{
	int	i;
	int	err;

	if (cmd->argv[1])
	{
		err = error_msg(1, "env", NULL, "no options or arguments supported");
		return (err);
	}
	i = 0;
	while (shell->env && shell->env[i])
	{
		if (has_equal(shell->env[i]))
		{
			write(1, shell->env[i], ft_strlen(shell->env[i]));
			write(1, "\n", 1);
		}
		i++;
	}
	return (0);
}
