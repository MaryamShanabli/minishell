#ifndef MINISHELL_H
#define MINISHELL_H

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>

extern int g_signal;

// Subject requirement: at most one global variable for signals.
// It must only store received signal number.

// extern volatile sig_atomic_t	g_signal;

typedef enum e_token_type
{
    T_WORD,         // normal text token (command, arg, filename, etc.)
    T_PIPE,         // |
    T_REDIR_IN,     // <
    T_REDIR_OUT,    // >
    T_HEREDOC,      // <<
    T_APPEND        // >>
}	t_token_type;

// typedef enum e_redir_type
// {
//     R_IN,           // input redirection: <
//     R_OUT,          // output redirection: >
//     R_HEREDOC,      // heredoc: <<
//     R_APPEND        // append output redirection: >>
// }	t_redir_type;

typedef struct s_token
{
    char			*value;      // token text, e.g. "echo", "file.txt", "|"
    t_token_type	type;       // token category from lexer
    struct s_token	*next;      // next token in linked list
}	t_token;

typedef struct s_redir
{
    e_token_type		type;        // which redirection kind this node is
    char				*target;     // filename or heredoc delimiter
    int					heredoc_fd;  // read-end fd for heredoc content (-1 if unused)
    struct s_redir		*next;       // next redirection for same command
}	t_redir;

typedef struct s_cmd
{
    char			**argv;        // NULL-terminated args array for execve/builtin
    t_redir			*redirs;       // all redirections belonging to this command
    // int			is_builtin;    // optional cached flag (not needed right now)
    pid_t			pid;           // child pid when forked (useful for wait)
    struct s_cmd	*next;         // next command in pipeline (after |)
}	t_cmd;

typedef struct s_env
{
    char			*key;          // env key, e.g. "PATH"
    char			*value;        // env value, e.g. "/usr/bin:/bin"
    int				exported;      // 1 if visible in env/export output
    struct s_env	*next;         // next env variable node
}	t_env;

typedef struct s_exec
{
    int		in_fd;              // current input fd for command execution
    int		out_fd;             // current output fd for command execution
    int		pipe_fd[2];         // pipe fds for current pipeline step
    int		prev_pipe_read;     // read-end from previous command pipe
}	t_exec;

typedef struct s_shell
{
    t_token	*tokens;             // token list from lexer
    t_cmd	*cmds;               // parsed command list (pipeline)
    t_env	*env;                // internal environment linked list

    int		last_status;         // last foreground status for $?
    int		interactive;         // 1 if shell is interactive (isatty)

    int		stdin_backup;        // saved stdin (for restore after redirs)
    int		stdout_backup;       // saved stdout (for restore after redirs)
}	t_shell;

char	**ft_split(char const *s, char c);
char	*ft_strdup(const char *s);
char	*ft_strjoin(const char *s1, const char *s2);
size_t	ft_strlen(const char *s);
void	dfree(char **arr);

char	*get_path(char *cmd);
t_cmd	process_input(char *input);
int		execute(t_cmd *cmd, char **envp, int last_status);
int		builtin_echo(t_cmd *cmd);
int		builtin_cd(t_cmd *cmd);
int		builtin_pwd(t_cmd *cmd);
int		builtin_export(t_cmd *cmd);
int		builtin_unset(t_cmd *cmd);
int		builtin_env(t_cmd *cmd);
int		builtin_exit(t_cmd *cmd, int last_status);
int		error_msg(int status, const char *cmd, const char *msg);
int		error_msg_arg(int status, const char *cmd,
            const char *arg, const char *msg);

void	exit_msg(int exit_code, const char *msg);

#endif
