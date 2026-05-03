/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_shlvl.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 17:24:43 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 17:47:35 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	reverse_str(char *buf, int len)
{
	int		i;
	int		j;
	char	tmp;

	i = 0;
	j = len - 1;
	while (i < j)
	{
		tmp = buf[i];
		buf[i] = buf[j];
		buf[j] = tmp;
		i++;
		j--;
	}
}

static int	int_to_buf(int n, char *buf, int size)
{
	int	i;

	i = 0;
	if (n == 0)
	{
		if (size > 1)
			buf[0] = '0';
		buf[1] = '\0';
		return (1);
	}
	while (n > 0 && i < size - 1)
	{
		buf[i++] = '0' + (n % 10);
		n /= 10;
	}
	buf[i] = '\0';
	reverse_str(buf, i);
	return (ft_strlen(buf));
}

static void	shlvl_warn(int lvl, char *buf)
{
	int	len;

	len = int_to_buf(lvl, buf, 8);
	write(2, "./minishell: warning: shell level (", 35);
	write(2, buf, (size_t)len);
	write(2, ") too high, resetting to 1\n", 27);
}

void	update_shlvl(t_shell *shell)
{
	char	*val;
	int		lvl;
	char	buf[8];

	val = env_get(shell->env, "SHLVL");
	if (!val)
		lvl = 1;
	else
		lvl = (int)ft_atol(val) + 1;
	if (lvl <= 0)
		lvl = 0;
	if (lvl >= 1000)
	{
		shlvl_warn(lvl, buf);
		lvl = 1;
	}
	int_to_buf(lvl, buf, sizeof(buf));
	env_set(&shell->env, "SHLVL", buf);
}
