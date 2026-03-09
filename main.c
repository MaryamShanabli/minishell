/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/03/10 00:27:16 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int g_signal = 0;

static void	handle_eof(int last_status)
{
	printf("exit\n");
	exit(last_status);
}

t_cmd	process_input(char *input)
{
	t_cmd	cmd;

	cmd.argv = NULL;
	cmd.redirs = NULL;
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
	t_shell	shell;

	shell.last_status = 0;

	if (ac > 1)
	{
		input = av[1];
		cmd = process_input(input);
		shell.last_status = execute(&cmd, envp, shell.last_status);
		if (cmd.argv)
			dfree(cmd.argv);
		return (shell.last_status);
	}
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			handle_eof(shell.last_status);
		if (*input)
			add_history(input);
		cmd = process_input(input);
		shell.last_status = execute(&cmd, envp, shell.last_status);
		if (cmd.argv)
			dfree(cmd.argv);
		free(input);
	}
	return (0);
}