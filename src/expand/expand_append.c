/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_append.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:43:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 15:59:56 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	append_str(t_expbuf *out, const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (!append_char(out, s[i]))
			return (0);
		i++;
	}
	return (1);
}
