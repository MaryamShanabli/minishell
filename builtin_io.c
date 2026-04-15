/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_io.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:11 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/15 12:49:37 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;
	int	j;

	i = 1;
	newline = 1;
	// ✅ FIXED: Skip ALL arguments that are ONLY "-n" characters (combined flags)
	// Examples: "-n", "-nn", "-nnn", "-nnnnn" are all treated as suppress-newline flags
	while (cmd->argv[i])
	{
		j = 0;
		// Check if argv[i] starts with '-'
		if (cmd->argv[i][j] != '-')
			break;
		j++;
		// Check if rest are all 'n' characters
		if (cmd->argv[i][j] == '\0')  // Just "-" alone
			break;
		while (cmd->argv[i][j] == 'n')
			j++;
		// If we consumed all characters and they were 'n's, it's a flag
		if (cmd->argv[i][j] == '\0')
		{
			newline = 0;
			i++;
		}
		else
			break;
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

	// ✅ ADDED: Counter to safely count number of arguments
	argc = 0;
	// ✅ ADDED: Loop through argv to count all arguments safely
	while (cmd->argv && cmd->argv[argc])
		argc++;
	// ✅ CHANGED: Check argc > 1 instead of argv[1]
	// pwd command accepts NO arguments, only the command itself (1 arg)
	if (argc > 1)
		return (error_msg(1, "pwd", NULL, "too many arguments"));
	if (!getcwd(cwd, sizeof(cwd)))
		return (perror("pwd"), 1);
	write(1, cwd, ft_strlen(cwd));
	write(1, "\n", 1);
	return (0);
}

int	builtin_env(t_cmd *cmd, t_shell *shell)
{
	int			i;
	int			has_equal;
	int			j;

	if (cmd->argv[1])
		return (error_msg(1, "env", NULL, "no options or arguments supported"));
	i = 0;
	while (shell->env && shell->env[i])
	{
		has_equal = 0;
		j = 0;
		while (shell->env[i][j])
		{
			if (shell->env[i][j] == '=')
			{
				has_equal = 1;
				break ;
			}
			j++;
		}
		if (has_equal)
		{
			write(1, shell->env[i], ft_strlen(shell->env[i]));
			write(1, "\n", 1);
		}
		i++;
	}
	return (0);
}
