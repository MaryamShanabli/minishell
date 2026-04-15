/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oalfoqha <oalfoqha@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:02 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/15 12:35:36 by oalfoqha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <limits.h>

static int	read_sign(const char *arg, int *i, int *sign)
{
	if (!arg || !arg[0])
		return (0);
	*i = 0;
	*sign = 1;
	if (arg[*i] == '+' || arg[*i] == '-')
	{
		if (arg[*i] == '-')
			*sign = -1;
		(*i)++;
	}
	if (!arg[*i])
		return (0);
	return (1);
}

static int	read_digits(const char *arg, int i,
		unsigned long long limit, unsigned long long *acc)
{
	int	digit;

	*acc = 0;
	while (arg[i])
	{
		digit = arg[i] - '0';
		if (digit < 0 || digit > 9)
			return (0);
		if (*acc > limit / 10 || (*acc == limit / 10
				&& (unsigned long long)digit > limit % 10))
			return (0);
		*acc = *acc * 10 + (unsigned long long)digit;
		i++;
	}
	return (1);
}

static int	parse_exit_code(const char *arg, long long *code)
{
	int					i;
	int					sign;
	unsigned long long	acc;
	unsigned long long	limit;

	if (!read_sign(arg, &i, &sign))
		return (0);
	if (arg[i] == '+' || arg[i] == '-')
		return (0);
	if (sign == -1)
		limit = (unsigned long long)LLONG_MAX + 1ULL;
	else
		limit = (unsigned long long)LLONG_MAX;
	if (!read_digits(arg, i, limit, &acc))
		return (0);
	if (sign == -1 && acc == (unsigned long long)LLONG_MAX + 1ULL)
		*code = LLONG_MIN;
	else if (sign == -1)
		*code = -(long long)acc;
	else
		*code = (long long)acc;
	return (1);
}

int	builtin_exit(t_cmd *cmd, int status)
{
	long long		code;
	int				argc;

	// ✅ ADDED: Counter to safely count number of arguments
	argc = 0;
	// ✅ ADDED: Loop through argv to count all arguments safely
	while (cmd->argv && cmd->argv[argc])
		argc++;
	if (isatty(STDIN_FILENO))
		write(2, "exit\n", 5);
	// ✅ CHANGED: Check argc == 1 instead of !cmd->argv[1]
	// If only 1 arg (command name "exit"), use last shell status
	if (argc == 1)
		return ((unsigned char)status);
	// Parse the exit code argument (argv[1])
	if (!parse_exit_code(cmd->argv[1], &code))
		return (error_msg(2, "exit", cmd->argv[1],
				"numeric argument required"));
	// ✅ CHANGED: Check argc > 2 instead of cmd->argv[2]
	// exit accepts max 2 args: "exit" (command) + exit_code
	if (argc > 2)
		return (error_msg(1, "exit", NULL, "too many arguments"));
	return ((unsigned char)code);
}
