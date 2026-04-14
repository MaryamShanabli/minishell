#ifndef MINISHELL_H
#define MINISHELL_H

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>

// Subject requirement: at most one global variable for signals.
// g_signal stores the last received signal number (e.g., SIGINT=2, SIGTERM=15).

extern volatile sig_atomic_t	g_signal;

typedef enum e_token_type
{
	T_COMMAND,		// command name (first word in command line)
	T_ARG,			// argument or option
	T_FILENAME,		// filename for redirection target
	T_PIPE,			// | (pipe operator)
	T_REDIR_IN,		// < (input redirection)
	T_REDIR_OUT,	// > (output redirection)
	T_HEREDOC,		// << (heredoc)
	T_APPEND		// >> (append output)
}	t_token_type;

typedef enum e_redir_type
{
    R_IN,           // input redirection: <
    R_OUT,          // output redirection: >
    R_HEREDOC,      // heredoc: <<
    R_APPEND        // append output redirection: >>
}	t_redir_type;

typedef struct s_token
{
    char			*value;      // token text, e.g. "echo", "file.txt", "|"
    t_token_type	type;       // token category from lexer
    struct s_token	*next;      // next token in linked list
}	t_token;

typedef struct s_redir
{
    t_redir_type		type;        // which redirection kind this node is
    char				*target;     // filename or heredoc delimiter
    int					heredoc_fd;  // read-end fd for heredoc content (-1 if unused)
    struct s_redir		*next;       // next redirection for same command
}	t_redir;

typedef struct s_cmd
{
    char			**argv;        // NULL-terminated args array for execve/builtin
    t_redir			*redirs;       // all redirections belonging to this command
    int			is_builtin;    // optional cached flag (not needed right now)
    pid_t			pid;           // child pid when forked (useful for wait)
    struct s_cmd	*next;         // next command in pipeline (after |)
}	t_cmd;

typedef struct s_exec
{
    int		in_fd;              // current input fd for command execution
    int		out_fd;             // current output fd for command execution
    int		pipe_fd[2];         // pipe fds for current pipeline step
    int		prev_pipe_read;     // read-end from previous command pipe
}	t_exec;

typedef struct s_shell
{
	char	**env;
	int		last_status;
}t_shell;

char	**ft_split(char const *s, char c);
char	*ft_strdup(const char *s);
char	*ft_strjoin(const char *s1, const char *s2);
char	*ft_strncpy(char *dst, const char *src, size_t n);
size_t	ft_strlen(const char *s);
int		ft_strcmp(const char *s1, const char *s2);
void	dfree(char **arr);
int		is_valid(const char *s);
char	**env_dup(char **src);
char	*env_get(char **env, const char *key);
int		env_set(char ***env, const char *key, const char *value);
int		env_unset(char ***env, const char *key);

char	*get_path(char *cmd, t_shell *shell);
t_cmd	process_input(char *input, t_shell *shell);
int		execute(t_cmd *cmd, t_shell *shell);
int		execute_pipeline(t_cmd *cmd, t_shell *shell);
int		apply_redirections(t_cmd *cmd);
int		is_builtin(char *name);
int		execute_builtin(t_cmd *cmd, t_shell *shell);
void		exec_child(t_cmd *cmd, t_shell *shell);
int		builtin_echo(t_cmd *cmd);
int		builtin_cd(t_cmd *cmd, t_shell *shell);
int		builtin_pwd(t_cmd *cmd);
int		builtin_export(t_cmd *cmd, t_shell *shell);
int		builtin_unset(t_cmd *cmd, t_shell *shell);
int		builtin_env(t_cmd *cmd, t_shell *shell);
int		builtin_exit(t_cmd *cmd, int status);
int		error_msg(int status, const char *cmd,
			const char *arg, const char *msg);
t_token *lexer(char *line);
void print_tokens(t_token *list);
void	expand_variables(t_token *tokens, t_shell *shell);

#endif
