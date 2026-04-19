/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_append.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:43:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/19 18:16:08 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	append_char2(t_expbuf *out, char c)
{
	char	*new_buf;

	if (out->used + 1 >= out->cap)
	{
		new_buf = malloc(out->cap * 2);
		if (!new_buf)
			return (0);
		ft_strncpy(new_buf, out->buf, out->used);
		free(out->buf);
		out->buf = new_buf;
		out->cap *= 2;
	}
	out->buf[out->used++] = c;
	return (1);
}

int	append_str(t_expbuf *out, const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (!append_char2(out, s[i]))
			return (0);
		i++;
	}
	return (1);
}
