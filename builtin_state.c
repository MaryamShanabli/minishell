











#include "minishell.h"

static void	print_export_entry(const char *entry)
{
	char	*eq;

	eq = ft_strchr(entry, '=');
	write(1, "declare -x ", 11);
	if (!eq)
	{
		write(1, entry, ft_strlen(entry));
		write(1, "\n", 1);
		return ;
	}
	write(1, entry, (size_t)(eq - entry));
	write(1, "=\"", 2);
	write(1, eq + 1, ft_strlen(eq + 1));
	write(1, "\"\n", 2);
}


static void	update_pwd_vars(t_shell *shell, char *oldpwd, char *newpwd)
{

	if (oldpwd)
		(void)env_set(&shell->env, "OLDPWD", oldpwd);

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



static int	go_to_home(t_shell *shell)
{
	char	*home;
	char	*oldpwd;
	char	*newpwd;


	home = env_get(shell->env, "HOME");
	if (!home || !*home)
		return (error_msg(1, "cd", NULL, "HOME not set"));

	oldpwd = getcwd(NULL, 0);

	if (chdir(home) == -1)
		return (free(oldpwd), error_msg(1, "cd", home, strerror(errno)));

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		cd_getcwd_error();

	update_pwd_vars(shell, oldpwd, newpwd);

	free(oldpwd);
	free(newpwd);
	return (0);
}


static int	go_to_path(const char *path, t_shell *shell)
{
	char	*oldpwd;
	char	*newpwd;


	oldpwd = getcwd(NULL, 0);

	if (chdir(path) == -1)
		return (free(oldpwd), error_msg(1, "cd", path, strerror(errno)));

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		cd_getcwd_error();

	update_pwd_vars(shell, oldpwd, newpwd);

	free(oldpwd);
	free(newpwd);
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{

	int	argc;


	argc = 0;

	if (!cmd->argv)
		return (1);


	while (cmd->argv[argc])
		argc++;


	if (argc > 2)
		return (error_msg(1, "cd", NULL, "too many arguments"));

	if (argc == 1)
		return (go_to_home(shell));

	if (!ft_strcmp(cmd->argv[1], "~") || !ft_strcmp(cmd->argv[1], "--"))
		return (go_to_home(shell));

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
			print_export_entry(shell->env[i]);
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
			if (!is_valid(arg))
			{
				error_msg(1, "export", cmd->argv[i], "not a valid identifier");
				status = 1;
			}
			else if (env_set(&shell->env, arg, eq_pos + 1))
				status = 1;
			*eq_pos = '=';
		}
		else if (!is_valid(arg))
		{
			error_msg(1, "export", arg, "not a valid identifier");
			status = 1;
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
