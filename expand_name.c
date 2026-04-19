/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_name.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:28:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 18:11:54 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	name_start(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static int	name_char(char c)
{
	return (name_start(c) || (c >= '0' && c <= '9'));
}

int	read_name(const char *in, size_t *pos, char *name)
{
	size_t	i;
	size_t	j;
	int		braced;

	i = *pos + 1;
	braced = (in[i] == '{');
	if (braced)
		i++;
	if (!name_start(in[i]))
		return (0);
	j = 0;
	while (name_char(in[i + j]))
	{
		if (j < 255)
			name[j] = in[i + j];
		j++;
	}
	if (braced && in[i + j] != '}')
		return (0);
	if (j > 255)
		j = 255;
	name[j] = '\0';
	*pos = i + j + braced;
	return (1);
}
