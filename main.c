/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/02/22 18:27:49 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int g_signal = 0;

static void	handle_eof(void)
{
	printf("exit\n");
	exit(0);
}

static void	process_input(char *input, char **envp)
{
	int		status;
	pid_t	p;

	if (!input || !*input)
	{
		free(input);
		return ;
	}
	add_history(input);
	p = fork();
	if (p == -1)
	{
		perror("fork");
		free(input);
		return ;
	}
	if (p == 0)
		execute(input, envp);
	else
	{
		waitpid(p, &status, 0);
		if (WIFEXITED(status))
			g_signal = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			g_signal = 128 + WTERMSIG(status);
	}
	free(input);
}

int	main(int argc, char **argv, char **envp)
{
	char	*input;

	if (argc > 1)
	{
		input = argv[1];
		process_input(ft_strdup(input), envp);
		return (g_signal);
	}
	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			handle_eof();
		process_input(input, envp);
	}
	return (0);
}