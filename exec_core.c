/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_core.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:49 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 15:12:50 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */













#include "minishell.h"

static void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

int	is_builtin(char *name)
{
	if (!name)
		return (0);
	if (!ft_strcmp(name, "echo") || !ft_strcmp(name, "cd")
		|| !ft_strcmp(name, "pwd") || !ft_strcmp(name, "export")
		|| !ft_strcmp(name, "unset") || !ft_strcmp(name, "env")
		|| !ft_strcmp(name, "exit"))
		return (1);
	return (0);
}

int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!ft_strcmp(cmd->argv[0], "echo"))
		return (builtin_echo(cmd));
	else if (!ft_strcmp(cmd->argv[0], "cd"))
		return (builtin_cd(cmd, shell));
	else if (!ft_strcmp(cmd->argv[0], "pwd"))
		return (builtin_pwd(cmd));
	else if (!ft_strcmp(cmd->argv[0], "export"))
		return (builtin_export(cmd, shell));
	else if (!ft_strcmp(cmd->argv[0], "unset"))
		return (builtin_unset(cmd, shell));
	else if (!ft_strcmp(cmd->argv[0], "env"))
		return (builtin_env(cmd, shell));
	else if (!ft_strcmp(cmd->argv[0], "exit"))
		return (builtin_exit(cmd, shell->last_status));
	return (1);
}

void	exec_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*slash;

	slash = ft_strchr(cmd->argv[0], '/');
	if (slash)
	{
		execve(cmd->argv[0], cmd->argv, shell->env);
		perror(cmd->argv[0]);
		if (errno == ENOENT)
			exit(127);
		exit(126);
	}
	path = get_path(cmd->argv[0], shell);
	if (!path)
		exit(error_msg(127, cmd->argv[0], NULL, "command not found"));
	execve(path, cmd->argv, shell->env);
	perror(path);
	free(path);
	if (errno == ENOENT)
		exit(127);
	exit(126);
}

static int	execute_external(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		cmd_status;

	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		set_child_signals();

		if (apply_redirections(cmd))
			exit(130);
		exec_child(cmd, shell);
	}
	waitpid(pid, &cmd_status, 0);
	if (WIFEXITED(cmd_status))
		return (WEXITSTATUS(cmd_status));
	else if (WIFSIGNALED(cmd_status))
	{
		if (WTERMSIG(cmd_status) == SIGQUIT)
			write(2, "Quit: 3\n", 8);
		else if (WTERMSIG(cmd_status) == SIGINT)
			write(2, "\n", 1);
		return (128 + WTERMSIG(cmd_status));
	}
	return (1);
}

static int	execute_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
	int	saved_in;
	int	saved_out;
	int	ret;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in == -1 || saved_out == -1)
		return (perror("dup"), 1);

	ret = apply_redirections(cmd);
	if (ret)
	{
		dup2(saved_in, STDIN_FILENO);
		dup2(saved_out, STDOUT_FILENO);
		close(saved_in);
		close(saved_out);
		return (ret);
	}
	else
		ret = execute_builtin(cmd, shell);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (ret);
}

static int	execute_redir_only(t_cmd *cmd)
{
	int	saved_in;
	int	saved_out;
	int	ret;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in == -1 || saved_out == -1)
		return (perror("dup"), 1);
	ret = apply_redirections(cmd);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (ret);
}

int	execute(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if (!cmd)
		return (shell->last_status);

	if ((!cmd->argv || !cmd->argv[0]) && cmd->redirs)
	{
		status = execute_redir_only(cmd);
		if (status != 0)
			return (status);

		if (cmd->next)
			return (execute_pipeline(cmd->next, shell));
		return (0);
	}

	if (!cmd->argv || !cmd->argv[0])
		return (shell->last_status);
	if (cmd->next)
		return (execute_pipeline(cmd, shell));
	if (is_builtin(cmd->argv[0]))
		return (execute_builtin_with_redir(cmd, shell));
	return (execute_external(cmd, shell));
}
