NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror -I.

SRC = src/core/main.c \
	src/exec/exec_core.c \
	src/exec/exec_builtin.c \
	src/exec/exec_path.c \
	src/exec/exec_heredoc.c \
	src/exec/exec_redir.c \
	src/exec/exec_pipe.c \
	src/builtin/builtin_io.c \
	src/builtin/builtin_state.c \
	src/builtin/builtin_unset.c \
	src/builtin/builtin_exit.c \
	src/utils/error_msg.c \
	src/parser/token_lexer.c \
	src/parser/token_builder.c \
	src/parser/shell_loop.c \
	src/parser/shell_redir.c \
	src/parser/shell_tokens.c \
	src/parser/shell_parser.c \
	src/parser/shell_memory.c \
	src/expand/expand_append.c \
	src/expand/expand_name.c \
	src/expand/expand_one.c \
	src/expand/expand_var.c \
	src/env/env_set.c \
	src/env/env_utils.c \
	src/utils/libft_split.c \
	src/utils/libft_memory.c \
	src/utils/libft_string.c
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