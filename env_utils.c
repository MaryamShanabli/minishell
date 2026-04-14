#include "minishell.h"

static int	env_key_match(const char *entry, const char *key)
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

static char	*make_env_entry(const char *key, const char *value)
{
	char	*tmp;
	char	*entry;

	if (!value)
		return (ft_strdup(key));
	tmp = ft_strjoin(key, "=");
	if (!tmp)
		return (NULL);
	entry = ft_strjoin(tmp, value);
	free(tmp);
	return (entry);
}

char	**env_dup(char **src)
{
	int		count;
	int		i;
	char	**dup;

	count = 0;
	while (src && src[count])
		count++;
	dup = malloc(sizeof(char *) * (count + 1));
	if (!dup)
		return (NULL);
	i = 0;
	while (i < count)
	{
		dup[i] = ft_strdup(src[i]);
		if (!dup[i])
			return (dfree(dup), NULL);
		i++;
	}
	dup[i] = NULL;
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
		if (env_key_match(entry, key))
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

int	env_set(char ***env, const char *key, const char *value)
{
	char	*entry;
	char	**new_env;
	int		i;
	int		j;

	if (!env || !key || !is_valid(key))
		return (1);
	entry = make_env_entry(key, value);
	if (!entry)
		return (1);
	i = 0;
	while ((*env) && (*env)[i])
	{
		if (env_key_match((*env)[i], key))
		{
			free((*env)[i]);
			(*env)[i] = entry;
			return (0);
		}
		i++;
	}
	new_env = malloc(sizeof(char *) * (i + 2));
	if (!new_env)
		return (free(entry), 1);
	j = 0;
	while (j < i)
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

int	env_unset(char ***env, const char *key)
{
	int	i;
	int	j;

	if (!env || !*env || !key)
		return (0);
	i = 0;
	while ((*env)[i])
	{
		if (env_key_match((*env)[i], key))
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
