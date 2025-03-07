/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:22:11 by modembel          #+#    #+#             */
/*   Updated: 2025/03/07 22:18:49 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	check_line(char *s, t_data *data, char **tmp)
{
	int	i;

	i = 0;
	if (!s)
	{
		data->tmps = tmp;
		exit_shell(data, g_exit_codes, 1);
	}
	while (s[i] && is_space(s[i]))
		i++;
	if (i == ft_strlen(s))
	{
		free_all(data);
		return (false);
	}
	if (!check_end(s, data))
	{
		ft_putstr_fd("error quote\n", 2);
		free_all(data);
		return (false);
	}
	return (true);
}

char	**no_env(void)
{
	char	buff[PATH_MAX];
	char	**arg;

	getcwd(buff, PATH_MAX);
	arg = malloc(sizeof(char *) * 4 + 1);
	arg[0] = ft_strjoin("PWD=", buff);
	arg[1] = "SHLVL=1";
	arg[2] = "_=/usr/bin/env";
	arg[3] = ft_strjoin("PATH=/usr/local/sbin:/usr/local/",
			"bin:/usr/sbin:/usr/bin:/sbin:/bin");
	arg[4] = NULL;
	return (arg);
}

char	**check_env(char **env, t_data *data)
{
	data->env_unset = false;
	if (!env[0])
	{
		data->env_unset = true;
		return (no_env());
	}
	return (env);
}

void	data_init(t_data *data, char **env)
{
	memset(data, 0, sizeof(t_data));
	data->env = check_env(env, data);
	data->cancel = false;
	data->end_quote = true;
	data->exit_code = 0;
	init_env(data->env, &data->envlist);
	data->save_stdin = dup(STDIN_FILENO);
	data->save_stdout = dup(STDOUT_FILENO);
}

int	main(int argc, char **argv, char **env)
{
	t_data	data;
	char	**tmp;

	(void)argc;
	(void)argv;
	tmp = NULL;
	while (1)
	{
		set_signal_parent();
		data_init(&data, env);
		data.line = readline("minishell>");
		add_history(data.line);
		if (!check_line(data.line, &data, tmp))
			continue ;
		data.tmps = tmp;
		lexer_expander(data.token, data.line, &data);
		if (create_command(&data))
			exec(&data, data.cmd);
		env = copy_arg_env(&data);
		free_all(&data);
		if (data.line)
			free(data.line);
		tmp = env;
	}
}

// 22