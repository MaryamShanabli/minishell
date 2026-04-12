#include "minishell.h"
#include <stdlib.h>

typedef struct s_expand
{
	const char	*in;
	char		*out_buf;
	size_t		in_pos;
	size_t		used_out;
	size_t		out_cap;
}t_expand;

static int	ft_isalpha(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static int	ft_isalnum(char c)
{
	return (ft_isalpha(c) || (c >= '0' && c <= '9'));
}

static void itoa_simple(int n, char *buf)
{
	int i = 0, j, sign = n;

	if (n == 0)
	{
		buf[i++] = '0';
		buf[i] = '\0';
		return;
	}
	if (n < 0)
		n = -n;
	while (n > 0)
	{
		buf[i++] = (n % 10) + '0';
		n /= 10;
	}
	if (sign < 0)
		buf[i++] = '-';
	buf[i] = '\0';
	for (j = 0, i--; j < i; j++, i--)
	{
		char tmp = buf[j];
		buf[j] = buf[i];
		buf[i] = tmp;
	}
}

static int	append_char(t_expand *exp, char c)
{
	char	*new_buf;
	size_t	new_cap;
	size_t	i;

	if (exp->used_out + 1 >= exp->out_cap)
	{
		new_cap = exp->out_cap * 2;
		new_buf = malloc(new_cap);
		if (!new_buf)
			return (0);
		i = 0;
		while (i < exp->used_out)
		{
			new_buf[i] = exp->out_buf[i];
			i++;
		}
		free(exp->out_buf);
		exp->out_buf = new_buf;
		exp->out_cap = new_cap;
	}
	exp->out_buf[(exp->used_out)++] = c;
	return (1);
}

static int	append_str(t_expand *exp, const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (!append_char(exp, s[i]))
			return (0);
		i++;
	}
	return (1);
}

static size_t	parse_name(const char *s, char *name, int braced)
{
	size_t	i;

	if (!s || !s[0])
		return (0);
	if (!((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z')
			|| s[0] == '_'))
		return (0);
	i = 0;
	while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
	{
		name[i] = s[i];
		i++;
	}
	if (braced)
	{
		if (s[i] != '}')
			return (0);
		name[i] = '\0';
		return (i + 1);
	}
	name[i] = '\0';
	return (i);
}

static int	handle_dollar(t_expand *exp, int last_status)
{
	char	name[256];
	char	nbr[12];
	const char	*val;
	size_t	consumed;

	if (exp->in[exp->in_pos + 1] == '\0')
		return (append_char(exp, '$'));
	if (exp->in[exp->in_pos + 1] == '?')
	{
		itoa_simple(last_status, nbr);
		exp->in_pos += 2;
		return (append_str(exp, nbr));
	}
	if (exp->in[exp->in_pos + 1] == '{')
	{
		consumed = parse_name(exp->in + exp->in_pos + 2, name, 1);
		if (consumed == 0)
			return (append_char(exp, exp->in[(exp->in_pos)++]));
		exp->in_pos += consumed + 2;
		val = getenv(name);
		return (append_str(exp, val));
	}
	consumed = parse_name(exp->in + exp->in_pos + 1, name, 0);
	if (consumed == 0)
		return (append_char(exp, exp->in[(exp->in_pos)++]));
	exp->in_pos += consumed + 1;
	val = getenv(name);
	return (append_str(exp, val));
}

static char *expand_one(const char *str, int last_status)
{
	t_expand	exp;
	int		in_single;
	int		in_double;

	exp.in = str;
	exp.out_cap = 64;
	exp.used_out = 0;
	exp.in_pos = 0;
	in_single = 0;
	in_double = 0;
	exp.out_buf = malloc(exp.out_cap);
	if (!exp.out_buf)
		return (NULL);

	while (exp.in[exp.in_pos])
	{
		if (exp.in[exp.in_pos] == '\'' && !in_double)
		{
			in_single = !in_single;
			exp.in_pos++;
			continue;
		}
		if (exp.in[exp.in_pos] == '"' && !in_single)
		{
			in_double = !in_double;
			exp.in_pos++;
			continue;
		}
		if (exp.in[exp.in_pos] == '$' && !in_single)
		{
			if (!handle_dollar(&exp, last_status))
			{
				free(exp.out_buf);
				return (NULL);
			}
			continue;
		}
		if (!append_char(&exp, exp.in[exp.in_pos]))
		{
			free(exp.out_buf);
			return (NULL);
		}
		exp.in_pos++;
	}
	if (!append_char(&exp, '\0'))
	{
		free(exp.out_buf);
		return (NULL);
	}
	return (exp.out_buf);
}

void expand_variables(t_token *tokens, int last_status)
{
	while (tokens)
	{
		if (tokens->type == T_COMMAND || tokens->type == T_ARG)
		{
			char *new_val = expand_one(tokens->value, last_status);
			if (!new_val)
				return;
			free(tokens->value);
			tokens->value = new_val;
		}
		tokens = tokens->next;
	}
}