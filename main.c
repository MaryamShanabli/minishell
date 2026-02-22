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