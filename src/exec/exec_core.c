/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_core.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:49 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/25 15:04:43 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>

static int	child_status(int cmd_status)
{
	int	status;

	status = 1;
	if (WIFEXITED(cmd_status))
		status = WEXITSTATUS(cmd_status);
	else if (WIFSIGNALED(cmd_status))
	{
		if (WTERMSIG(cmd_status) == SIGQUIT)
			write(2, "Quit\n", 5);
		else if (WTERMSIG(cmd_status) == SIGINT)
			write(2, "\n", 1);
		status = 128 + WTERMSIG(cmd_status);
	}
	return (status);
}

static int	exec_no_argv(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if ((!cmd->argv || !cmd->argv[0]) && cmd->redirs)
	{
		status = execute_redir_only(cmd);
		if (status)
			return (status);
		if (cmd->next)
		{
			status = execute_pipeline(cmd->next, shell);
			return (status);
		}
		return (0);
	}
	if (!cmd->argv || !cmd->argv[0])
		return (shell->last_status);
	return (-1);
}

void	exec_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*slash;
	int		err;
	struct stat st;

	slash = ft_strchr(cmd->argv[0], '/');
	if (slash)
	{
		if (stat(cmd->argv[0], &st) == 0 && S_ISDIR(st.st_mode))
		{
			err = error_msg(126, cmd->argv[0], NULL, "Is a directory");
			exit(err);
		}
		execve(cmd->argv[0], cmd->argv, shell->env);
		perror(cmd->argv[0]);
		if (errno == ENOENT)
			exit(127);
		exit(126);
	}
	path = get_path(cmd->argv[0], shell);
	if (!path)
	{
		err = error_msg(127, cmd->argv[0], NULL, "command not found");
		exit(err);
	}
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
	int		status;
	void	(*old_int)(int);
	void	(*old_quit)(int);

	old_int = signal(SIGINT, SIG_IGN);
	old_quit = signal(SIGQUIT, SIG_IGN);
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		signal(SIGINT, old_int);
		signal(SIGQUIT, old_quit);
		return (1);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (apply_redirections(cmd))
			exit(130);
		exec_child(cmd, shell);
	}
	waitpid(pid, &cmd_status, 0);
	signal(SIGINT, old_int);
	signal(SIGQUIT, old_quit);
	status = child_status(cmd_status);
	return (status);
}

int	execute(t_cmd *cmd, t_shell *shell)
{
	int	status;

	if (!cmd)
		return (shell->last_status);
	status = exec_no_argv(cmd, shell);
	if (status != -1)
		return (status);
	if (cmd->next)
	{
		status = execute_pipeline(cmd, shell);
		return (status);
	}
	if (is_builtin(cmd->argv[0]))
	{
		status = execute_builtin_with_redir(cmd, shell);
		return (status);
	}
	status = execute_external(cmd, shell);
	return (status);
}
