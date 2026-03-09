#include "minishell.h"

int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->argv[1] && !strcmp(cmd->argv[1], "-n"))
	{
		newline = 0;
		i++;
	}
	while (cmd->argv[i])
	{
		printf("%s", cmd->argv[i]);
		if (cmd->argv[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

int	builtin_pwd(t_cmd *cmd)
{
	char	cwd[1024];

	if (cmd->argv[1])
		return (error_msg(1, "pwd", "too many arguments"));
	if (!getcwd(cwd, sizeof(cwd)))
		return (perror("pwd"), 1);
	printf("%s\n", cwd);
	return (0);
}

int	builtin_env(t_cmd *cmd)
{
	extern char	**environ;
	int			i;

	if (cmd->argv[1])
		return (error_msg(1, "env", "no options or arguments supported"));
	i = 0;
	while (environ[i])
	{
		printf("%s\n", environ[i]);
		i++;
	}
	return (0);
}
