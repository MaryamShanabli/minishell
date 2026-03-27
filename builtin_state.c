/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 20:04:19 by mshanabl          #+#    #+#             */
/*   Updated: 2026/03/27 20:04:20 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_cd(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "cd", "not implemented yet"));
}

int	builtin_export(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "export", "not implemented yet"));
}

int	builtin_unset(t_cmd *cmd)
{
	(void)cmd;
	return (error_msg(1, "unset", "not implemented yet"));
}
