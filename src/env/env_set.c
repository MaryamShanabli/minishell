/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_set.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 16:30:00 by mshanabl          #+#    #+#             */
/*   Updated: 2026/05/12 00:00:00 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_has(char **env, const char *key)
{
	int	i;

	i = 0;
	while (env && env[i])
	{
		if (key_match(env[i], key))
			return (1);
		i++;
	}
	return (0);
}

static char	*make_entry(const char *key, const char *value)
{
	char	*tmp;
	char	*entry;

	if (!value)
	{
		entry = ft_strdup(key);
		return (entry);
	}
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	return (entry);
}

static int	append_entry(char ***env, char *entry, int count)
{
	char	**new_env;
	int		j;

	new_env = malloc(sizeof(char *) * (count + 2));
	if (!new_env)
	{
		free(entry);
		return (1);
	}
	j = 0;
	while (j < count)
	{
		new_env[j] = (*env)[j];
		j++;
	}
	new_env[j] = entry;
	new_env[j + 1] = NULL;
	free(*env);
	*env = new_env;
	return (0);
}

int	env_set(char ***env, const char *key, const char *value)
{
	char	*entry;
	int		i;
	int		status;

	if (!env || !key || !is_valid(key))
		return (1);
	entry = make_entry(key, value);
	if (!entry)
		return (1);
	i = 0;
	while ((*env) && (*env)[i])
	{
		if (key_match((*env)[i], key))
		{
			free((*env)[i]);
			(*env)[i] = entry;
			return (0);
		}
		i++;
	}
	status = append_entry(env, entry, i);
	return (status);
}
