#include "minishell.h"

// int execute_pipeline(t_cmd *cmd, char **envp, int last_status)
// {
// 	t_exec	exec;

// 	if (pipe(exec.pipe_fd) == -1)
// 	{
// 		perror("pipe");
// 		return (1);
// 	}
// 	exec.in_fd = 0;
// 	exec.prev_pipe_read = -1;
// 	while (cmd)
// 	{
// 		if()
// 	}

// } 
/* TODO: Implement pipeline execution
 * One fork per command, pipes connecting stdout->stdin between commands
 * Extract execute_cmd_in_child() helper to avoid code duplication
 * Handle redirections in each child process
 */
int	execute_pipeline(t_cmd *cmd, char **envp, int last_status)
{
	(void)cmd;
	(void)envp;
	return (last_status);
}