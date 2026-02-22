#include "minishell.h"

int	main(int argc, char **argv, char **envp)
{
	int		status;
	pid_t	p;

	if (argc < 2)
	{
		printf("Usage: %s <command>\n", argv[0]);
		return (1);
	}
	p = fork();
	if (p == -1)
	{
		perror("fork");
		return (1);
	}
	if (p == 0)
	{
		execute(argv[1], envp);
	}
	waitpid(p, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (0);
}

// #ifndef MINISHELL_H
// #define MINISHELL_H

// #include <stddef.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <string.h>
// #include <errno.h>
// #include <sys/types.h>
// #include <sys/wait.h>

// char	**ft_split(char const *s, char c);
// char	*ft_strdup(const char *s);
// char	*ft_strjoin(const char *s1, const char *s2);
// size_t	ft_strlen(const char *s);
// void	dfree(char **arr);

// char	*get_path(char *cmd);
// void	execute(char *cmd, char **envp);

// #endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 16:55:22 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/02/22 16:55:42 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int g_signal = 0;

static void	handle_eof(void)
{
	printf("exit\n");
	exit(0);
}

static void	process_input(char *input)
{
	if (*input)
		add_history(input);

	free(input);
}

int	main(void)
{
	char	*input;

	while (1)
	{
		input = readline("minishell$ ");
		if (!input)
			handle_eof();
		process_input(input);
	}
	return (0);
}
