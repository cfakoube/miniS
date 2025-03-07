/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfakoube <cfakoube@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:35:23 by modembel          #+#    #+#             */
/*   Updated: 2025/03/03 07:25:57 by cfakoube         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*tmp_oldpwd(void)
{
	char	buff[PATH_MAX];
	char	*tmp;

	if (getcwd(buff, PATH_MAX))
		tmp = ft_strjoin("OLDPWD=", buff);
	return (tmp);
}

int	get_pwd(t_data *data, char *oldpwd)
{
	char	buff[PATH_MAX];
	char	*tmp;
	t_env	*env;

	env = data->envlist;
	data->old_pwd = ft_strdup(oldpwd);
	update_env(&env, var_exist("OLDPWD=", &env), oldpwd);
	if (var_exist("OLDPWD=", &env) == -1)
		add_env(&env, data->old_pwd);
	if (!getcwd(buff, PATH_MAX))
	{
		printf("%s\n", strerror(errno));
		return (-1);
	}
	tmp = ft_strjoin("PWD=", buff);
	data->pwd = ft_strdup(tmp);
	update_env(&env, var_exist("PWD=", &env), tmp);
	if (var_exist("PWD=", &env) == -1)
		add_env(&env, data->pwd);
	free(tmp);
	return (0);
}

char	*get_home(t_data *data)
{
	t_env	*env;
	char	*dest;

	dest = NULL;
	env = data->envlist;
	while (env)
	{
		if (ft_strncmp(env->str, "HOME", 4) == 0)
			dest = ft_substr(env->str, 5, INT_MAX);
		env = env->next;
	}
	if (!dest)
		ft_putstr_fd("HOME not set\n", 2);
	return (dest);
}

int	change_direc(t_cmd *cmd, char *home)
{
	if (cmd->param[1] && cmd->param[2])
	{
		ft_putstr_fd("cd: to many argument\n", 2);
		return (1);
	}
	if (!cmd->param[1])
	{
		cmd->param[1] = home;
		if (!cmd->param[1])
			return (1);
	}
	if (chdir(cmd->param[1]) == -1)
	{
		printf("%s\n", strerror(errno));
		return (1);
	}
	return (0);
}

int	ft_cd(t_cmd *cmd, t_data *data)
{
	char	*str;

	str = tmp_oldpwd();
	data->home = get_home(data);
	if (change_direc(cmd, data->home) == 1)
	{
		if (data->home)
			free(data->home);
		free(str);
		return (1);
	}
	if (get_pwd(data, str) == -1)
	{
		if (data->home)
			free(data->home);
		free(str);
		return (1);
	}
	if (data->home)
		free(data->home);
	free(str);
	return (0);
}
