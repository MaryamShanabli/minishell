/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 15:12:38 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/12 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	key_match(const char *entry, const char *key)
{
	int	i;

	i = 0;
	if (!entry || !key)
		return (0);
	while (key[i] && entry[i] && key[i] == entry[i])
		i++;
	if (!key[i] && (entry[i] == '=' || entry[i] == '\0'))
		return (1);
	return (0);
}

static char	**env_dup_fill(char **src, char **dup, int count)
{
	char	*item;
	int		i;

	i = 0;
	while (i < count)
	{
		item = ft_strdup(src[i]);
		if (!item)
		{
			dfree(dup);
			return (NULL);
		}
		dup[i] = item;
		i++;
	}
	dup[i] = NULL;
	return (dup);
}

char	**env_dup(char **src)
{
	int		count;
	char	**dup;

	count = 0;
	while (src && src[count])
		count++;
	dup = calloc(count + 1, sizeof(char *));
	if (!dup)
		return (NULL);
	dup = env_dup_fill(src, dup, count);
	return (dup);
}

char	*env_get(char **env, const char *key)
{
	int		i;
	int		j;
	char	*entry;

	i = 0;
	while (env && env[i])
	{
		entry = env[i];
		if (key_match(entry, key))
		{
			j = 0;
			while (entry[j] && entry[j] != '=')
				j++;
			if (entry[j] == '=')
				return (entry + j + 1);
			return (NULL);
		}
		i++;
	}
	return (NULL);
}

int	env_unset(char ***env, const char *key)
{
	int	i;
	int	j;

	if (!env || !*env || !key)
		return (0);
	i = 0;
	while ((*env)[i])
	{
		if (key_match((*env)[i], key))
		{
			free((*env)[i]);
			j = i;
			while ((*env)[j])
			{
				(*env)[j] = (*env)[j + 1];
				j++;
			}
			return (0);
		}
		i++;
	}
	return (0);
}
