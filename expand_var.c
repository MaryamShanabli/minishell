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

static int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
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

/* Added for testcase: export x=" ismail" then echo $x => ismail, echo "$x" =>  ismail */
static int	has_quotes(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
	{
		if (s[i] == '\'' || s[i] == '"')
			return (1);
		i++;
	}
	return (0);
}

/* Added for testcase: trim unquoted expansion edge whitespace (IFS-like) */
static char	*trim_ifs_edges(const char *s)
{
	size_t	start;
	size_t	end;
	char	*out;

	if (!s)
		return (ft_strdup(""));
	start = 0;
	while (s[start] == ' ' || s[start] == '\t' || s[start] == '\n')
		start++;
	end = ft_strlen(s);
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'
			|| s[end - 1] == '\n'))
		end--;
	out = malloc(end - start + 1);
	if (!out)
		return (NULL);
	ft_strncpy(out, s + start, end - start);
	out[end - start] = '\0';
	return (out);
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

static int	handle_dollar(t_expand *exp, t_shell *shell)
{
	char	name[256];
	char	nbr[12];
	const char	*val;
	size_t	consumed;

	if (exp->in[exp->in_pos + 1] == '\0')
		return (append_char(exp, '$'));
	if (exp->in[exp->in_pos + 1] == '?')
	{
		itoa_simple(shell->last_status, nbr);
		exp->in_pos += 2;
		return (append_str(exp, nbr));
	}
	if (ft_isdigit(exp->in[exp->in_pos + 1]))
	{
		if (exp->in[exp->in_pos + 1] == '0')
		{
			exp->in_pos += 2;
			return (append_str(exp, "./minishell"));
		}
		exp->in_pos += 2;
		return (1);
	}
	if (exp->in[exp->in_pos + 1] == '{')
	{
		consumed = parse_name(exp->in + exp->in_pos + 2, name, 1);
		if (consumed == 0)
			return (append_char(exp, exp->in[(exp->in_pos)++]));
		exp->in_pos += consumed + 2;
		val = env_get(shell->env, name);
		return (append_str(exp, val));
	}
	consumed = parse_name(exp->in + exp->in_pos + 1, name, 0);
	if (consumed == 0)
		return (append_char(exp, exp->in[(exp->in_pos)++]));
	exp->in_pos += consumed + 1;
	val = env_get(shell->env, name);
	return (append_str(exp, val));
}

static char *expand_one(const char *str, t_shell *shell)
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
			if (!handle_dollar(&exp, shell))
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

void	expand_variables(t_token *tokens, t_shell *shell)
{
	char	*new_val;
	char	*trimmed;
	int		quoted;

	while (tokens)
	{
		if (tokens->type == T_COMMAND || tokens->type == T_ARG)
		{
			quoted = has_quotes(tokens->value);
			new_val = expand_one(tokens->value, shell);
			if (!new_val)
				return;
			if (!quoted)
			{
				trimmed = trim_ifs_edges(new_val);
				free(new_val);
				if (!trimmed)
					return;
				new_val = trimmed;
				tokens->remove_if_empty = (new_val[0] == '\0'); /* testcase: $non_exist and space-only vars */
			}
			else
				tokens->remove_if_empty = 0;
			free(tokens->value);
			tokens->value = new_val;
		}
		tokens = tokens->next;
	}
}