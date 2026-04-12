/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:19 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/12 20:10:57 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	key_match(const char *env_entry, const char *key)
{
	int	i;

	i = 0;
	if (!env_entry || !key)
		return (0);
	while (key[i] && env_entry[i] && key[i] == env_entry[i])
		i++;
	if (!key[i] && (env_entry[i] == '=' || env_entry[i] == '\0'))
		return (1);
	return (0);
}

static char	*make_env_entry(const char *key, const char *value)
{
	char	*tmp;
	char	*entry;

	if (!value)
		return (ft_strdup(key));
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	return (entry);
}

static int update_env_var(char **envp, const char *key, const char *value)
{
	extern char	**environ;
	int		i;
	char	*entry;
	int		exists;
	char	**new_envp;
	int		j;

	(void)envp;
	if (!environ || !key)
		return (1);
	if (!is_valid(key))
		return (error_msg(1, "export", key, "not a valid identifier"));
	entry = make_env_entry(key, value);
	if (!entry)
		return (1);
	exists = 0;
	i = 0;
	while (environ[i])
	{
		if (key_match(environ[i], key))
		{
			exists = 1;
			environ[i] = entry;
			break ;
		}
		i++;
	}
	if (!exists)
	{
		new_envp = malloc(sizeof(char *) * (i + 2));
		if (!new_envp)
		{
			free(entry);
			return (1);
		}
		j = 0;
		while (j < i)
		{
			new_envp[j] = environ[j];
			j++;
		}
		new_envp[j] = entry;
		new_envp[j + 1] = NULL;
		environ = new_envp;
	}
	return (0);
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

int	builtin_cd(t_cmd *cmd, char **envp)
{
	char	*target;
	char	*oldpwd;
	char	*newpwd;
	int		warned;

	warned = 0;
	if (cmd->argv[2])
		return (error_msg(1, "cd", NULL, "too many arguments"));
	if (!cmd->argv[1]
		|| !ft_strcmp(cmd->argv[1], "~")
		|| !ft_strcmp(cmd->argv[1], "--"))
	{
		target = getenv("HOME");
		if (!target || !*target)
			return (error_msg(1, "cd", NULL, "HOME not set"));
	}
	else
		target = cmd->argv[1];
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd && !warned)
	{
		cd_getcwd_error();
		warned = 1;
	}
	if (chdir(target) == -1)
	{
		free(oldpwd);
		return (error_msg(1, "cd", target, strerror(errno)));
	}
	newpwd = getcwd(NULL, 0);
	if (!newpwd && !warned)
	{
		cd_getcwd_error();
		warned = 1;
	}
	if (oldpwd)
		(void)update_env_var(envp, "OLDPWD", oldpwd);
	if (newpwd)
		(void)update_env_var(envp, "PWD", newpwd);
	free(oldpwd);
	free(newpwd);
	return (0);
}

int	builtin_export(t_cmd *cmd, char **envp)
{
	extern char	**environ;
	int		i;
	char	*arg;
	char	*eq_pos;
	int		status;

	arg = cmd->argv[1];
	if (!arg)
	{
		i = 0;
		while (environ[i])
		{
			write(1, "declare -x ", 11);
			write(1, environ[i], ft_strlen(environ[i]));
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
		eq_pos = strchr(arg, '=');
		if (eq_pos)
		{
			*eq_pos = '\0';
			if (update_env_var(envp, arg, eq_pos + 1))
				status = 1;
			*eq_pos = '=';
		}
		else if (update_env_var(envp, arg, NULL))
			status = 1;
		i++;
	}
	return (status);
}

int	builtin_unset(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "unset", NULL, "not implemented yet"));
}
