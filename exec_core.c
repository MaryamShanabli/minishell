/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_core.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:39 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/11 14:55:42 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	execute_builtin(t_cmd *cmd, int status, char **envp)
{
	if (!ft_strcmp(cmd->argv[0], "echo"))
		return (builtin_echo(cmd));
	else if (!ft_strcmp(cmd->argv[0], "cd"))
		return (builtin_cd(cmd, envp));
	else if (!ft_strcmp(cmd->argv[0], "pwd"))
		return (builtin_pwd(cmd));
	else if (!ft_strcmp(cmd->argv[0], "export"))
		return (builtin_export(cmd, envp));
	else if (!ft_strcmp(cmd->argv[0], "unset"))
		return (builtin_unset(cmd));
	else if (!ft_strcmp(cmd->argv[0], "env"))
		return (builtin_env(cmd));
	else if (!ft_strcmp(cmd->argv[0], "exit"))
		return (builtin_exit(cmd, status));
	return (1);
}

void	exec_child(t_cmd *cmd, char **envp)
{
	char	*path;

	path = get_path(cmd->argv[0]);
	if (!path)
		exit(error_msg(127, cmd->argv[0], NULL, "command not found"));
	execve(path, cmd->argv, envp);
	perror(path);
	free(path);
	if (errno == ENOENT)
		exit(127);
	exit(126);
}

static int	execute_external(t_cmd *cmd, char **envp)
{
	pid_t	pid;
	int		cmd_status;

	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		/* Plan: apply single-command redirections in child before execve. */
		if (apply_redirections(cmd))
			exit(1);
		exec_child(cmd, envp);
	}
	waitpid(pid, &cmd_status, 0);
	if (WIFEXITED(cmd_status))
		return (WEXITSTATUS(cmd_status));
	else if (WIFSIGNALED(cmd_status))
		return (128 + WTERMSIG(cmd_status));
	return (1);
}

static int	execute_builtin_with_redir(t_cmd *cmd, int status, char **envp)
{
	int	saved_in;
	int	saved_out;
	int	ret;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in == -1 || saved_out == -1)
		return (perror("dup"), 1);
	/* Plan: run builtins with temporary fd redirection in parent context. */
	if (apply_redirections(cmd))
		ret = 1;
	else
		ret = execute_builtin(cmd, status, envp);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (ret);
}

int	execute(t_cmd *cmd, char **envp, int status)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (status);
	if (cmd->next)
		return (execute_pipeline(cmd, envp, status));
	if (is_builtin(cmd->argv[0]))
		return (execute_builtin_with_redir(cmd, status, envp));
	return (execute_external(cmd, envp));
}
