/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 18:43:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/04 18:21:13 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	append_char(t_expbuf *out, char c)
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
		if (!append_char(out, s[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	name_start(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static int	name_char(char c)
{
	int	r;

	r = name_start(c) || (c >= '0' && c <= '9');
	return (r);
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
