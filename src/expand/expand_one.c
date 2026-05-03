/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_one.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:13:06 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 18:26:11 by mshanabl         ###   ########.fr       */
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

static int	dollar_special(const char *in, size_t *pos, t_expbuf *out,
		t_shell *shell)
{
	char	nbr[12];
	int		err;

	if (in[*pos + 1] == '\0')
	{
		err = append_char(out, in[*pos]);
		(*pos)++;
		return (err);
	}
	if (in[*pos + 1] == '?')
	{
		ft_itoa_buf(shell->last_status, nbr);
		*pos += 2;
		err = append_str(out, nbr);
		return (err);
	}
	if (in[*pos + 1] >= '0' && in[*pos + 1] <= '9')
	{
		err = 1;
		if (in[*pos + 1] == '0')
			err = append_str(out, "./minishell");
		*pos += 2;
		return (err);
	}
	return (-1);
}

static int	handle_dollar(const char *in, size_t *pos, t_expbuf *out,
		t_shell *shell)
{
	char		name[256];
	const char	*val;
	int			err;

	err = dollar_special(in, pos, out, shell);
	if (err != -1)
		return (err);
	err = read_name(in, pos, name);
	if (!err)
	{
		err = append_char(out, in[*pos]);
		(*pos)++;
		return (err);
	}
	val = env_get(shell->env, name);
	err = append_str(out, val);
	return (err);
}

static int	expand_step(const char *str, t_expand_state *st,
		t_expbuf *out, t_shell *shell)
{
	int	err;

	err = 1;
	if (str[st->pos] == '\'' && !st->in_double)
	{
		st->in_single = !st->in_single;
		st->pos++;
	}
	else if (str[st->pos] == '"' && !st->in_single)
	{
		st->in_double = !st->in_double;
		st->pos++;
	}
	else if (str[st->pos] == '$' && !st->in_single)
		err = handle_dollar(str, &st->pos, out, shell);
	else
	{
		err = append_char(out, str[st->pos]);
		st->pos++;
	}
	return (err);
}

char	*expand_one(const char *str, t_shell *shell)
{
	t_expbuf		out;
	t_expand_state	state;
	int				err;

	out.cap = 64;
	out.used = 0;
	out.buf = malloc(out.cap);
	if (!out.buf)
		return (NULL);
	state.pos = 0;
	state.in_single = 0;
	state.in_double = 0;
	err = 1;
	while (str[state.pos] && err)
		err = expand_step(str, &state, &out, shell);
	if (err)
		err = append_char(&out, '\0');
	if (!err)
	{
		free(out.buf);
		return (NULL);
	}
	return (out.buf);
}
