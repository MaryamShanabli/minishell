#include "minishell.h"

static int	is_numeric_arg(const char *arg)
{
	int	i;

	if (!arg || !arg[0])
		return (0);
	i = 0;
	if (arg[i] == '+' || arg[i] == '-')
		i++;
	if (!arg[i])
		return (0);
	while (arg[i])
	{
		if (arg[i] < '0' || arg[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(t_cmd *cmd, int last_status)
{
	char			*end;
	long long	code;

	if (isatty(STDIN_FILENO))
		write(2, "exit\n", 5);
	if (!cmd->argv[1])
		exit((unsigned char)last_status);
	if (!is_numeric_arg(cmd->argv[1]))
		exit(error_msg_arg(2, "exit", cmd->argv[1],
				"numeric argument required"));
	if (cmd->argv[2])
		return (error_msg(1, "exit", "too many arguments"));
	errno = 0;
	code = strtoll(cmd->argv[1], &end, 10);
	if (errno == ERANGE || (end && *end))
		exit(error_msg_arg(2, "exit", cmd->argv[1],
				"numeric argument required"));
	exit((unsigned char)code);
}
