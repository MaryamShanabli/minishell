#include "minishell.h"

static int	is_builtin(char *name)
{
	if (!name)
		return (0);
	if (!ft_strcmp(name, "echo") || !ft_strcmp(name, "cd") || !ft_strcmp(name, "pwd")
		|| !ft_strcmp(name, "export") || !ft_strcmp(name, "unset")
		|| !ft_strcmp(name, "env") || !ft_strcmp(name, "exit"))
		return (1);
	return (0);
}

static int	execute_builtin(t_cmd *cmd, int last_status)
{
	if (!ft_strcmp(cmd->argv[0], "echo"))
		return (builtin_echo(cmd));
	else if (!ft_strcmp(cmd->argv[0], "cd"))
		return (builtin_cd(cmd));
	else if (!ft_strcmp(cmd->argv[0], "pwd"))
		return (builtin_pwd(cmd));
	else if (!ft_strcmp(cmd->argv[0], "export"))
		return (builtin_export(cmd));
	else if (!ft_strcmp(cmd->argv[0], "unset"))
		return (builtin_unset(cmd));
	else if (!ft_strcmp(cmd->argv[0], "env"))
		return (builtin_env(cmd));
	else if (!ft_strcmp(cmd->argv[0], "exit"))
		return (builtin_exit(cmd, last_status));
	return (1);
}

static int	execute_external(t_cmd *cmd, char **envp)
{
	char	*path;
	int		status;
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		path = get_path(cmd->argv[0]);
		if (!path)
			exit(error_msg(127, cmd->argv[0], "command not found"));
		execve(path, cmd->argv, envp);
		perror(path);
		free(path);
		if (errno == ENOENT)
			exit(127);
		exit(126);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

int	execute(t_cmd *cmd, char **envp, int last_status)
{
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return (last_status);
    if (cmd->next)
        return (execute_pipeline(cmd, envp, last_status));
    if (is_builtin(cmd->argv[0]))
        return (execute_builtin(cmd, last_status));
    return (execute_external(cmd, envp));
}

