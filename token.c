#include "minishell.h"
#include <string.h>

void	add_token(t_token **list, t_token *new)
{
	t_token	*tmp;

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

t_token	*new_token(char *value, t_token_type type)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->value = strdup(value);
	tok->type = type;
	tok->next = NULL;
	return (tok);
}

char	*read_word(char *line, int *i)
{
	int		start;
	int		len;
	char	*word;

	start = *i;
	while (line[*i]
		&& line[*i] != ' '
		&& line[*i] != '|'
		&& line[*i] != '>'
		&& line[*i] != '<')
		(*i)++;
	len = *i - start;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	strncpy(word, line + start, len);
	word[len] = '\0';
	return (word);
}

t_token	*lexer(char *line)
{
	int		i;
	t_token	*tokens;

	i = 0;
	tokens = NULL;
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
			add_token(&tokens, new_token(word, T_WORD));
			free(word);
		}
	}
	return (tokens);
}

void	print_tokens(t_token *list)
{
	while (list)
	{
		printf("TOKEN: %-10s TYPE: %d\n", list->value, list->type);
		list = list->next;
	}
}