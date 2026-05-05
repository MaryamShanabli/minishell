/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/02 22:26:56 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 20:31:05 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_export(const char *entry)
{
	char	*eq;

	eq = ft_strchr(entry, '=');
	write(1, "declare -x ", 11);
	if (!eq)
	{
		write(1, entry, ft_strlen(entry));
		write(1, "\n", 1);
		return ;
	}
	write(1, entry, (size_t)(eq - entry));
	write(1, "=\"", 2);
	write(1, eq + 1, ft_strlen(eq + 1));
	write(1, "\"\n", 2);
}

static int	export_one_arg(char *arg, t_shell *shell)
{
	char	*eq_pos;
	int		err;

	eq_pos = ft_strchr(arg, '=');
	err = 0;
	if (!eq_pos && !is_valid(arg))
		err = error_msg(1, "export", arg, "not a valid identifier");
	else if (!eq_pos && env_set(&shell->env, arg, NULL))
		err = 1;
	else if (eq_pos)
	{
		*eq_pos = '\0';
		if (!is_valid(arg))
			err = error_msg(1, "export", arg, "not a valid identifier");
		else if (env_set(&shell->env, arg, eq_pos + 1))
			err = 1;
		*eq_pos = '=';
	}
	return (err);
}

static void	print_sorted_env(char **env, int n)
{
	int		i;
	int		j;
	char	*tmp;

	i = 0;
	while (i < n - 1)
	{
		j = 0;
		while (j < n - 1 - i)
		{
			if (ft_strcmp(env[j], env[j + 1]) > 0)
			{
				tmp = env[j];
				env[j] = env[j + 1];
				env[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
	i = 0;
	while (env[i])
		print_export(env[i++]);
}

static int	export_print_all(t_shell *shell)
{
	char	**sorted;
	int		n;
	int		i;

	n = 0;
	while (shell->env && shell->env[n])
		n++;
	sorted = malloc(sizeof(char *) * (n + 1));
	if (!sorted)
		return (1);
	i = -1;
	while (++i < n)
		sorted[i] = shell->env[i];
	sorted[n] = NULL;
	print_sorted_env(sorted, n);
	free(sorted);
	return (0);
}

int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int	status;
	int	i;
	int	ret;

	if (!cmd->argv[1])
	{
		ret = export_print_all(shell);
		return (ret);
	}
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (export_one_arg(cmd->argv[i], shell))
			status = 1;
		i++;
	}
	return (status);
}
