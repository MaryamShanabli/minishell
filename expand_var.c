#include "minishell.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

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

static char *expand_one(const char *str, int last_status)
{
	size_t len = strlen(str);
	char *result = malloc(len * 8 + 64);
	size_t ri = 0;
	int in_single = 0;

	if (!result)
	{
	
		return NULL;
	}

	for (size_t i = 0; i < len;)
	{
		if (str[i] == '\'')
		{
			in_single = !in_single;
			i++;
			continue;
		}
		if (str[i] == '"')
		{
			i++;
			continue;
		}
		if (str[i] == '$' && !in_single)
		{
			if (str[i + 1] == '?')
			{
				char buf[16];
				itoa_simple(last_status, buf);
				size_t l = strlen(buf);
				memcpy(result + ri, buf, l);
				ri += l;
				i += 2;
				continue;
			}
			size_t var_start = i + 1;
			size_t var_len = 0;
			while (str[var_start + var_len] &&
				(isalnum((unsigned char)str[var_start + var_len]) ||
				str[var_start + var_len] == '_'))
				var_len++;
			if (var_len == 0)
			{
				result[ri++] = str[i++];
				continue;
			}
			char *var = malloc(var_len + 1);
			if (!var)
			{
				free(result);
				return NULL;
			}
			strncpy(var, str + var_start, var_len);
			var[var_len] = '\0';
			const char *val = getenv(var);
			free(var);
			if (val)
			{
				size_t l = strlen(val);
				memcpy(result + ri, val, l);
				ri += l;
			}
			i = var_start + var_len;
			continue;
		}
		result[ri++] = str[i++];
	}
	result[ri] = '\0';
	return result;
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