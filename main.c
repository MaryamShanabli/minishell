/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/03/27 18:36:08 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t g_signal = 0;

static void	handle_eof(int last_status)
{
	if (isatty(STDIN_FILENO))
		write(1, "exit\n", 5);
	(void)last_status;
}

t_cmd	process_input(char *input)
{
	t_cmd	cmd;

	cmd.argv = NULL;	
	cmd.pid = 0;
	cmd.next = NULL;
	if (!input)
		return (cmd);
	cmd.argv = ft_split(input, ' ');
	if (!cmd.argv)
		return (cmd);
	if (!cmd.argv[0])
	{
		dfree(cmd.argv);
		cmd.argv = NULL;
	}
	return (cmd);
}

int	main(int ac, char **av, char **envp)
{
	char	*input;
	t_cmd	cmd;
	int status;

	status = 0;
	if (ac > 1)
	{
		input = av[1];
		cmd = process_input(input);
		status = execute(&cmd, envp, status);
		if (cmd.argv)
			dfree(cmd.argv);
		return (status);
	}
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
		{
			handle_eof(status);
			break ;
		}
		add_history(input);
		cmd = process_input(input);
		status = execute(&cmd, envp, status);
		if (cmd.argv && !ft_strcmp(cmd.argv[0], "exit"))
		{
			dfree(cmd.argv);
			free(input);
			break ;
		}
		if (cmd.argv)
			dfree(cmd.argv);
		free(input);
	}
	exit(status);
}