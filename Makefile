NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c \
	exec_core.c \
	exec_builtin.c \
	exec_path.c \
	exec_heredoc.c \
	exec_redir.c \
	exec_pipe.c \
	builtin_io.c \
	builtin_state.c \
	builtin_unset.c \
	builtin_exit.c \
	error_msg.c \
	token_lexer.c \
	token_builder.c \
	shell_loop.c \
	shell_redir.c \
	shell_parse_utils.c \
	shell_parser.c \
	shell_memory.c \
	expand_append.c \
	expand_name.c \
	expand_one.c \
	expand_var.c \
	env_set.c \
	env_utils.c \
	ft_split.c \
	utils1.c \
	utils2.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re