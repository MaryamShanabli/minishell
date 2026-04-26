/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dot.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 14:45:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/04/26 18:38:14 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	dot_error_missing(void)
{
	write(2, "minishell: .: filename argument required\n", 41);
	write(2, "minishell: .: usage: . filename [arguments]\n", 44);
	return (2);
}

static int	exec_dot_line(char *line, t_shell *shell)
{
	t_cmd	cmd;
	int		status;

	if (!line || !*line)
		return (0);
	cmd = process_input(line, shell);
	if (cmd.pid == -2)
	{
		free_cmd_list(&cmd);
		return (2);
	}
	status = execute(&cmd, shell);
	shell->last_status = status;
	free_cmd_list(&cmd);
	return (status);
}

static int	append_char(char **line, size_t *len, char c)
{
	char	*new_line;

	new_line = malloc(*len + 2);
	if (!new_line)
		return (0);
	if (*line)
	{
		ft_strncpy(new_line, *line, *len);
		free(*line);
	}
	new_line[*len] = c;
	new_line[*len + 1] = '\0';
	*line = new_line;
	(*len)++;
	return (1);
}

static int	dot_run_file(const char *path, t_shell *shell)
{
	int		fd;
	char	buf[256];
	ssize_t	rd;
	char	*line;
	size_t	len;
	int		status;
	struct stat st;

	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
		return (error_msg(1, ".", path, "is a directory"));
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (error_msg(1, ".", path, strerror(errno)));
	line = NULL;
	len = 0;
	status = 0;
	while ((rd = read(fd, buf, sizeof(buf))) > 0)
	{
		size_t i;

		i = 0;
		while (i < (size_t)rd)
		{
			if (buf[i] == '\n')
			{
				status = exec_dot_line(line, shell);
				free(line);
				line = NULL;
				len = 0;
			}
			else if (!append_char(&line, &len, buf[i]))
			{
				free(line);
				close(fd);
				return (1);
			}
			i++;
		}
	}
	if (rd == -1)
		status = error_msg(1, ".", path, strerror(errno));
	else if (line)
		status = exec_dot_line(line, shell);
	free(line);
	close(fd);
	return (status);
}

int	builtin_dot(t_cmd *cmd, t_shell *shell)
{
	if (!cmd->argv[1])
		return (dot_error_missing());
	return (dot_run_file(cmd->argv[1], shell));
}
