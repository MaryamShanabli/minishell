NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRC = main.c \
	exec_core.c \
	exec_path.c \
	exec_redir.c \
	exec_pipe.c \
	builtin_io.c \
	builtin_state.c \
	builtin_exit.c \
	error_msg.c \
	token.c \
	expand_var.c \
	ft_split.c \
	utils.c
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