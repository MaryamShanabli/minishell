#include "minishell.h"

int	builtin_cd(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "cd", "not implemented yet"));
}

int	builtin_export(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "export", "not implemented yet"));
}

int	builtin_unset(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "unset", "not implemented yet"));
}
