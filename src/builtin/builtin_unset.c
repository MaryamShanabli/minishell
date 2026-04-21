/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 16:00:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 16:08:52 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	int		status;
	char	*arg;

	if (!cmd || !cmd->argv || !cmd->argv[1])
		return (0);
	status = 0;
	i = 1;
	while (cmd->argv[i])
	{
		arg = cmd->argv[i];
		if (!is_valid(arg))
		{
			error_msg(1, "unset", arg, "not a valid identifier");
			status = 1;
		}
		else
			(void)env_unset(&shell->env, arg);
		i++;
	}
	return (status);
}
