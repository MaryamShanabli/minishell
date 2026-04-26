/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:40:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/04/26 18:38:21 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
#include <sys/stat.h>

extern volatile sig_atomic_t	g_signal;

typedef enum e_token_type
{
	T_COMMAND,
	T_ARG,
	T_FILENAME,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_HEREDOC,
	T_APPEND
}	t_token_type;

typedef enum e_redir_type
{
	R_IN,
	R_OUT,
	R_HEREDOC,
	R_APPEND
}	t_redir_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	int				remove_if_empty;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	t_redir_type		type;
	char				*target;
	int					heredoc_fd;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
	int				is_builtin;
	pid_t			pid;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_exec
{
	int	in_fd;
	int	out_fd;
	int	pipe_fd[2];
	int	prev_pipe_read;
}	t_exec;

typedef struct s_shell
{
	char	**env;
	int		last_status;
}	t_shell;

typedef struct s_expbuf
{
	char	*buf;
	size_t	used;
	size_t	cap;
}	t_expbuf;

typedef struct s_expand_state
{
	size_t	pos;
	int		in_single;
	int		in_double;
}	t_expand_state;

char	**ft_split(char const *s, char c);
char	*ft_strdup(const char *s);
char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_strncpy(char *dst, const char *src, size_t n);
char	*ft_strchr(const char *s, int c);
size_t	ft_strlen(const char *s);
int		ft_strcmp(const char *s1, const char *s2);
void	dfree(char **arr);
int		is_valid(const char *s);

int		key_match(const char *entry, const char *key);
char	**env_dup(char **src);
char	*env_get(char **env, const char *key);
int		env_set(char ***env, const char *key, const char *value);
int		env_unset(char ***env, const char *key);

int		error_msg(int status, const char *cmd,
			const char *arg, const char *msg);

int		is_builtin(char *name);
int		execute_builtin(t_cmd *cmd, t_shell *shell);
int		execute_builtin_with_redir(t_cmd *cmd, t_shell *shell);
int		execute_redir_only(t_cmd *cmd);

int		builtin_echo(t_cmd *cmd);
int		builtin_pwd(t_cmd *cmd);
int		builtin_env(t_cmd *cmd, t_shell *shell);
int		builtin_cd(t_cmd *cmd, t_shell *shell);
int		builtin_export(t_cmd *cmd, t_shell *shell);
int		builtin_dot(t_cmd *cmd, t_shell *shell);
int		builtin_unset(t_cmd *cmd, t_shell *shell);
int		builtin_exit(t_cmd *cmd, int status);

char	*get_path(char *cmd, t_shell *shell);
int		execute(t_cmd *cmd, t_shell *shell);
void	exec_child(t_cmd *cmd, t_shell *shell);
int		execute_pipeline(t_cmd *cmd, t_shell *shell);
int		apply_redirections(t_cmd *cmd);
int		do_heredoc(const char *delim);

t_token	*lexer(char *line);
char	*read_word(char *line, int *i);
int		add_operator_token(char *line, int *i, t_token **tokens,
			int *first_word);
int		add_word_token(char *line, int *i, t_token **tokens,
			int *first_word);
void	print_tokens(t_token *list);

char	*expand_one(const char *str, t_shell *shell);
int		append_str(t_expbuf *out, const char *s);
int		read_name(const char *in, size_t *pos, char *name);
void	expand_variables(t_token *tokens, t_shell *shell);

void	init_cmd(t_cmd *cmd);
t_cmd	process_input(char *input, t_shell *shell);
int		interactive_loop(t_shell *shell);
int		is_redir_token(t_token_type type);
int		parse_redirection(t_cmd *cmd, t_token **it);
int		handle_token(t_token **it, t_cmd **current, char ***argv, int *argc);
void	free_tokens(t_token *tokens);
void	free_cmd_list(t_cmd *cmd);

#endif
