/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_itoa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 17:29:46 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/03 17:46:45 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	reverse_buf(char *buf, int len)
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

void	ft_itoa_buf(int n, char *buf)
{
	int	i;

	i = 0;
	if (n == 0)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return ;
	}
	while (n > 0)
	{
		buf[i++] = '0' + (n % 10);
		n /= 10;
	}
	buf[i] = '\0';
	reverse_buf(buf, i);
}
