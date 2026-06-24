/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 16:40:00 by oalfoqha          #+#    #+#             */
/*   Updated: 2026/06/24 13:52:41 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stddef.h>
# include <stdlib.h>
# include <string.h>
# include <termios.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <sys/stat.h>

extern volatile sig_atomic_t	g_signal;

typedef enum e_token_type
{
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
	int					heredoc_expand;
	int					target_was_unquoted_var;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char			**argv;
	t_redir			*redirs;
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
	int		should_exit;
	int		exit_code;
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

typedef struct s_hd_finish
{
	pid_t			pid;
	int				fd[2];
	void			(*old_int)(int);
	void			(*old_quit)(int);
	struct termios	old_term;
}	t_hd_finish;

typedef struct s_pipe_sig
{
	struct sigaction	sa_ign;
	struct sigaction	old_int;
	struct sigaction	old_quit;
}	t_pipe_sig;

typedef struct s_pipe_scan
{
	char	quote;
	char	prev;
	char	last;
	int		has_non_pipe;
}	t_pipe_scan;

char	**ft_split(char const *s, char c);
char	*ft_strdup(const char *s);
char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_strncpy(char *dst, const char *src, size_t n);
char	*ft_strchr(const char *s, int c);
size_t	ft_strlen(const char *s);
int		ft_strcmp(const char *s1, const char *s2);
void	dfree(char **arr);
int		is_valid(const char *s);
long	ft_atol(const char *s);
void	ft_bzero(void *s, size_t n);
int		itoa_buf(int n, char *buf, int size);

int		key_match(const char *entry, const char *key);
char	**env_dup(char **src);
char	*env_get(char **env, const char *key);
int		env_set(char ***env, const char *key, const char *value);
int		env_unset(char ***env, const char *key);
int		env_has(char **env, const char *key);

int		error_msg(int status, char *cmd,
			char *arg, char *msg);
int		syntax_token_error(const char *token);

int		is_builtin(char *name);
int		execute_builtin(t_cmd *cmd, t_shell *shell);
int		execute_builtin_with_redir(t_cmd *cmd, t_shell *shell);
int		execute_external(t_cmd *cmd, t_shell *shell);

int		builtin_echo(t_cmd *cmd);
int		builtin_pwd(t_cmd *cmd);
int		builtin_env(t_cmd *cmd, t_shell *shell);
int		builtin_cd(t_cmd *cmd, t_shell *shell);
int		builtin_export(t_cmd *cmd, t_shell *shell);
int		builtin_unset(t_cmd *cmd, t_shell *shell);
int		builtin_exit(t_cmd *cmd, t_shell *shell);

char	*get_path(char *cmd, t_shell *shell);
int		execute(t_cmd *cmd, t_shell *shell);
void	exec_child(t_cmd *cmd, t_shell *shell);
int		execute_pipeline(t_cmd *cmd, t_shell *shell);
void	exec_pipe(t_cmd *cmd, t_exec *exec, t_shell *shell);
void	close_heredoc_fds(t_cmd *cmd);
int		apply_redirections(t_cmd *cmd);
int		preprocess_heredocs(t_cmd *cmd, t_shell *shell);
int		do_heredoc_pre(t_redir *redir, t_shell *shell);
void	child_reset_signals(void);
void	pipe_sig_setup(t_pipe_sig *sig);
void	heredoc_loop(int write_fd, const char *delim, t_shell *shell,
			int expand);
void	pipe_child(t_cmd *cmd, t_exec *exec, t_shell *shell);

t_token	*lexer(char *line);
char	*read_word(char *line, int *i);
int		add_operator_token(char *line, int *i, t_token **tokens);
int		add_word_token(char *line, int *i, t_token **tokens);

char	*expand_one(const char *str, t_shell *shell);
int		append_str(t_expbuf *out, const char *s);
int		read_name(const char *in, size_t *pos, char *name);
void	expand_variables(t_token **head, t_shell *shell);
t_token	*word_split_token(t_token *tok, t_token *next);
int		append_char(t_expbuf *out, char c);
void	update_shlvl(t_shell *shell);

void	init_cmd(t_cmd *cmd);
t_cmd	*process_input(char *input, t_shell *shell);
int		interactive_loop(t_shell *shell);
int		consume_sigint(t_shell *shell);
void	sigint_display_fix(void);
int		handle_sigint_empty(t_shell *shell, char *input);
char	*append_line(char *input, char *next);
int		read_pipe_continuation(char **input);
int		read_quote_continuation(char **input);
int		prompt_loop(t_shell *shell);
int		is_redir_token(t_token_type type);
int		add_redirection(t_cmd *cmd, t_token_type type,
			const char *target, int was_unquoted_var);
int		parse_redirection(t_cmd *cmd, t_token **it);
char	**alloc_argv(t_token *it, int *count_out);
int		count_cmd_args(t_token *it);
int		handle_token(t_token **it, t_cmd **current, char ***argv, int *argc);
void	free_tokens(t_token *tokens);
void	free_cmd_list(t_cmd *cmd);
void	free_redirs(t_redir *redirs);
void	handle_eof(int status);

void	print_minishell_masterpiece(void);

#endif
