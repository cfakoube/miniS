/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expanser.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfakoube <cfakoube@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:34:20 by cfakoube          #+#    #+#             */
/*   Updated: 2025/03/07 03:39:07 by cfakoube         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*copy_env(t_env *envi, char *var)
{
	int		size;
	int		len;
	t_env	*env;

	env = envi;
	len = 0;
	size = ft_strlen(var);
	while (env)
	{
		if (ft_strncmp(env->str, var, size) == 0)
		{
			while (env->str[len] && env->str[len] != '=')
				len++;
			return (ft_strdup(env->str + len + 1));
		}
		env = env->next;
	}
	return (NULL);
}

char	*update_string(char *str, char *replace, char *var, int index)
{
	int		size;
	char	*dest;
	int		i;
	int		j;

	i = 0;
	j = 0;
	size = ft_strlen(str) - (ft_strlen(var) + 1) + ft_strlen(replace);
	dest = malloc(sizeof(char) * size + 1);
	if (!dest)
		return (NULL);
	while (str[i])
	{
		if (str[i] == '$' && i == index)
			i += seconde_update_string(&j, dest, replace, var);
		if (!str[i])
			break ;
		dest[j] = str[i];
		i++;
		j++;
	}
	dest[j] = '\0';
	return (dest);
}

char	*before_dollar(char *str)
{
	int		i;
	char	*dest;

	i = 0;
	while (str[i] && str[i] != '$')
		i++;
	dest = malloc(sizeof(char) * i + 1);
	if (!dest)
		return (NULL);
	i = 0;
	while (str[i] && str[i] != '$')
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*special_cases(char *copy, char *var, int index)
{
	char	*replace;
	char	*str;

	if (var[0] == '?')
		replace = ft_itoa(g_exit_codes);
	else
		replace = ft_itoa(getpid());
	str = update_string(copy, replace, var, index);
	free(replace);
	free(var);
	free(copy);
	return (str);
}

void	set_var_value(char **str, t_env *env, int index)
{
	char	*var;
	char	*replace;
	char	*copy;

	copy = *str;
	var = var_isolation(copy, index);
	if (ft_strlen(var) < 1)
		return (free(var));
	if (var_in_env(var, env))
	{
		replace = copy_env(env, var);
		*str = update_string(copy, replace, var, index);
		clean_expansion(var, replace, copy);
	}
	else if (var[0] == '?' || var[0] == '$')
		*str = special_cases(copy, var, index);
	else if (!var_in_env(var, env) && var[0])
	{
		replace = ft_strdup("");
		*str = update_string(copy, replace, var, index);
		clean_expansion(var, replace, copy);
	}
}

void	set_var_valuee(char **str, t_env *env, int index, bool *skip)
{
	char	*var;
	char	*replace;
	char	*copy;

	copy = *str;
	var = var_isolation(copy, index);
	if (ft_strlen(var) < 1)
		return (free(var));
	if (var_in_env(var, env))
	{
		replace = copy_env(env, var);
		*str = update_string(copy, replace, var, index);
		clean_expansion(var, replace, copy);
	}
	else if (var[0] == '?' || var[0] == '$')
		*str = special_cases(copy, var, index);
	else if (!var_in_env(var, env) && var[0])
	{
		replace = ft_strdup("");
		*str = update_string(copy, replace, var, index);
		clean_expansion(var, replace, copy);
		*skip = true;
	}
}

static int	for_the_norm(char c, bool q)
{
	if (q && (c == '\'' || (c == '\"' || (c == '$'))))
			// dollar will probably generate error
		return (1);
	return (0);
}

char	*var_expansion_two(char *str, t_data *data, int *i, bool *skip)
{
	int		j;
	char	c;
	char	*dest;

	j = *i;
	dest = str;
	if (dest[j] == '\'' || dest[j] == '\"')
	{
		c = dest[j];
		j++;
		while (dest[j] && dest[j] != c)
		{
			if (dest[j] == '$' && c != '\'')
				set_var_valuee(&dest, data->envlist, j, skip);
			if (*skip && dest[j] == '$')
				continue ;
			else
				j++;
		}
	}
	*i = j;
	return (dest);
}

void	var_expansion(t_data *data)
{
	t_token	*tok;
	int		i;
	bool	q;

	tok = data->token;
	while (tok)
	{
		i = 0;
		if (tok->type == VAR)
		{
			while (tok->str[i])
			{
				q = false;
				tok->str = var_expansion_two(tok->str, data, &i, &q);
				if (tok->str[i] == '$')
					set_var_valuee(&tok->str, data->envlist, i, &q);
				if (tok->str[i] && for_the_norm(tok->str[i], q) == 0)
					i++;
			}
		}
		tok = tok->next;
	}
}
