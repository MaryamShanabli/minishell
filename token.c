#include "minishell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void add_token(t_token **list, t_token *new)
{
	t_token *tmp;

	if (!new)
		return;
	if (!*list)
	{
		*list = new;
		return;
	}
	tmp = *list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
}

t_token *new_token(char *value, t_token_type type)
{
	t_token *tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return NULL;
	tok->value = ft_strdup(value);
	tok->type = type;
	tok->remove_if_empty = 0; /* testcase default: only expander can mark this */
	tok->next = NULL;
	return tok;
}

char *read_word(char *line, int *i)
{
	int		start;
	int		len;
	char	quote;
	char	*word;

	start = *i;
	quote = 0;
	while (line[*i])
	{
		if (quote)
		{
			if (line[*i] == quote)
				quote = 0;
			(*i)++;
		}
		else
		{
			if (line[*i] == '\'' || line[*i] == '"')
			{
				quote = line[*i];
				(*i)++;
			}
			else if (line[*i] == ' ' || line[*i] == '|'
				|| line[*i] == '>' || line[*i] == '<')
				break;
			else
				(*i)++;
		}
	}
	len = *i - start;
	word = malloc(len + 1);
	if (!word)
		return NULL;
	ft_strncpy(word, line + start, len);
	word[len] = '\0';
	return word;
}

t_token *lexer(char *line)
{
	int i = 0;
	t_token *tokens = NULL;
	int first_word = 1;

	while (line[i])
	{
		while (line[i] == ' ')
			i++;
		if (!line[i])
			break;
		if (line[i] == '|')
		{
			add_token(&tokens, new_token("|", T_PIPE));
			i++;
			first_word = 1;
		}
		else if (line[i] == '>' && line[i + 1] == '>')
		{
			add_token(&tokens, new_token(">>", T_APPEND));
			i += 2;
		}
		else if (line[i] == '>')
		{
			add_token(&tokens, new_token(">", T_REDIR_OUT));
			i++;
		}
		else if (line[i] == '<' && line[i + 1] == '<')
		{
			add_token(&tokens, new_token("<<", T_HEREDOC));
			i += 2;
		}
		else if (line[i] == '<')
		{
			add_token(&tokens, new_token("<", T_REDIR_IN));
			i++;
		}
		else
		{
			char *word = read_word(line, &i);
			if (!word || !word[0])
			{
				if (word)
					free(word);
				continue;
			}
			t_token_type type = first_word ? T_COMMAND : T_ARG;
			add_token(&tokens, new_token(word, type));
			first_word = 0;
			free(word);
		}
	}
	return tokens;
}

void print_tokens(t_token *list)
{
	while (list)
	{
		printf("TOKEN: %-10s TYPE: %d\n", list->value, list->type);
		list = list->next;
	}
}