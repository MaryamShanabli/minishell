/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:20 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/12 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	skip_sign(const char *s, int *sign)
{
	int	i;

	i = 0;
	*sign = 1;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			*sign = -1;
		i++;
	}
	return (i);
}

static int	parse_digits(const char *s, int i, unsigned long long *out)
{
	unsigned long long	acc;
	int					d;

	acc = 0;
	if (!s[i])
		return (0);
	while (s[i])
	{
		d = s[i] - '0';
		if (d < 0 || d > 9)
			return (0);
		if (acc > ULLONG_MAX / 10
			|| (acc == ULLONG_MAX / 10
				&& (unsigned long long)d > ULLONG_MAX % 10))
			return (0);
		acc = acc * 10 + (unsigned long long)d;
		i++;
	}
	*out = acc;
	return (1);
}

static int	parse_exit_code(const char *raw, long long *code)
{
	int					sign;
	int					i;
	unsigned long long	val;

	while (*raw == ' ' || *raw == '\t')
		raw++;
	if (!*raw)
		return (0);
	i = skip_sign(raw, &sign);
	if (!parse_digits(raw, i, &val))
		return (0);
	if (sign == 1)
	{
		if (val > (unsigned long long)LLONG_MAX)
			return (0);
		*code = (long long)val;
		return (1);
	}
	if (val > (unsigned long long)LLONG_MAX + 1ULL)
		return (0);
	if (val == (unsigned long long)LLONG_MAX + 1ULL)
		*code = LLONG_MIN;
	else
		*code = -(long long)val;
	return (1);
}

static int	resolve_code(t_cmd *cmd, t_shell *shell, long long *code)
{
	int	argc;

	argc = 0;
	while (cmd->argv && cmd->argv[argc])
		argc++;
	if (argc == 1)
		*code = (unsigned char)shell->last_status;
	else
	{
		if (!parse_exit_code(cmd->argv[1], code))
		{
			error_msg(2, "exit", cmd->argv[1], "numeric argument required");
			shell->should_exit = 1;
			shell->exit_code = 2;
			return (2);
		}
		if (argc > 2)
		{
			error_msg(1, "exit", NULL, "too many arguments");
			return (1);
		}
		*code = (unsigned char)*code;
	}
	return (0);
}

int	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	long long	code;
	int			ret;

	if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
		write(2, "exit\n", 5);
	ret = resolve_code(cmd, shell, &code);
	if (ret == 1)
		return (1);
	if (ret == 2)
		return (2);
	shell->should_exit = 1;
	shell->exit_code = (int)code;
	return ((int)code);
}
