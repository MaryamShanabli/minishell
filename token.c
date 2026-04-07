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
	tok->next = NULL;
	return tok;
}

char *read_word(char *line, int *i)
{
	int start = *i;
	int len = 0;
	char quote = 0;
	char *word;

	if (line[*i] == '\'' || line[*i] == '"')
	{
		quote = line[*i];
		int j = *i + 1;
		while (line[j] && line[j] != quote)
			j++;
		len = j - *i + (line[j] == quote ? 1 : 0);
		word = malloc(len + 1);
		if (!word)
			return NULL;
		ft_strncpy(word, line + *i, len);
		word[len] = '\0';
		*i += len;
		return word;
	}
	start = *i;
	while (line[*i] && line[*i] != ' ' && line[*i] != '|' && line[*i] != '>' && line[*i] != '<' && line[*i] != '\'' && line[*i] != '"')
		(*i)++;
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