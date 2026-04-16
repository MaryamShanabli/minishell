/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:19 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/16 15:50:56 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// ✅ ADDED: Helper function to update PWD and OLDPWD environment variables
static void	update_pwd_vars(t_shell *shell, char *oldpwd, char *newpwd)
{
	// Update OLDPWD with the previous directory
	if (oldpwd)
		(void)env_set(&shell->env, "OLDPWD", oldpwd);
	// Update PWD with the new current directory
	if (newpwd)
		(void)env_set(&shell->env, "PWD", newpwd);
}

static void	cd_getcwd_error(void)
{
	char	*msg;
	char	*full;

	msg = strerror(errno);
	full = ft_strjoin("getcwd: cannot access parent directories: ", msg);
	if (!full)
	{
		error_msg(1, "cd", "error retrieving current directory", msg);
		return ;
	}
	error_msg(1, "cd", "error retrieving current directory", full);
	free(full);
}

// ✅ ADDED: Helper function to change to HOME directory
// Handles: cd, cd ~, cd --
static int	go_to_home(t_shell *shell)
{
	char	*home;
	char	*oldpwd;
	char	*newpwd;

	// Get HOME from environment
	home = env_get(shell->env, "HOME");
	if (!home || !*home)
		return (error_msg(1, "cd", NULL, "HOME not set"));
	// Save current directory before changing
	oldpwd = getcwd(NULL, 0);
	// Change to HOME directory
	if (chdir(home) == -1)
		return (free(oldpwd), error_msg(1, "cd", home, strerror(errno)));
	// Get the new current directory
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		cd_getcwd_error();
	// Update PWD and OLDPWD environment variables
	update_pwd_vars(shell, oldpwd, newpwd);
	// Free allocated memory
	free(oldpwd);
	free(newpwd);
	return (0);
}

// ✅ ADDED: Helper function to change to a specific path
static int	go_to_path(const char *path, t_shell *shell)
{
	char	*oldpwd;
	char	*newpwd;

	// Save current directory before changing
	oldpwd = getcwd(NULL, 0);
	// Try to change to the specified path
	if (chdir(path) == -1)
		return (free(oldpwd), error_msg(1, "cd", path, strerror(errno)));
	// Get the new current directory
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		cd_getcwd_error();
	// Update PWD and OLDPWD environment variables
	update_pwd_vars(shell, oldpwd, newpwd);
	// Free allocated memory
	free(oldpwd);
	free(newpwd);
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	// ✅ ADDED: Counter variable to safely track number of arguments
	int	argc;

	// ✅ ADDED: Initialize counter to zero
	argc = 0;
	// ✅ ADDED: Safety check - verify argv exists before using it
	if (!cmd->argv)
		return (1);
	// ✅ ADDED: Loop through argv to count all arguments safely
	// This is safer than checking argv[1] or argv[2] directly
	while (cmd->argv[argc])
		argc++;
	// ✅ ADDED: Use argc count instead of checking argv[2]
	// cd command accepts max 2 arguments: "cd" (command name) + 1 path
	if (argc > 2)
		return (error_msg(1, "cd", NULL, "too many arguments"));
	// ✅ ADDED: If only 1 argument (just "cd"), go to HOME directory
	if (argc == 1)
		return (go_to_home(shell));
	// ✅ KEPT: Check if argument 1 is "~" or "--" (special HOME cases)
	if (!ft_strcmp(cmd->argv[1], "~") || !ft_strcmp(cmd->argv[1], "--"))
		return (go_to_home(shell));
	// ✅ KEPT: Otherwise go to the specified path (argv[1])
	return (go_to_path(cmd->argv[1], shell));
}

int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*arg;
	char	*eq_pos;
	int		status;

	arg = cmd->argv[1];
	if (!arg)
	{
		i = 0;
		while (shell->env && shell->env[i])
		{
			write(1, "declare -x ", 11);
			write(1, shell->env[i], ft_strlen(shell->env[i]));
			write(1, "\n", 1);
			i++;
		}
		return (0);
	}
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		arg = cmd->argv[i];
		eq_pos = ft_strchr(arg, '=');
		if (eq_pos)
		{
			*eq_pos = '\0';
			if (env_set(&shell->env, arg, eq_pos + 1))
				status = 1;
			*eq_pos = '=';
		}
		else if (env_set(&shell->env, arg, NULL))
			status = 1;
		i++;
	}
	return (status);
}

int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	int		status;
	char	*arg;

	if (!cmd || !cmd->argv || !cmd->argv[1])
		return (0);
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		arg = cmd->argv[i];
		if (!is_valid(arg))
		{
			error_msg(1, "unset", arg, "not a valid identifier");
			status = 1;
		}
		else
			(void)env_unset(&shell->env, arg);
		i++;
	}
	return (status);
}
