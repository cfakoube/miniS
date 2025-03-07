/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfakoube <cfakoube@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 20:44:36 by modembel          #+#    #+#             */
/*   Updated: 2025/03/07 03:37:46 by cfakoube         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	write_in_heredoc(int fd, char *line)
{
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	if (line)
		free(line);
}

bool	get_line(int fd, char *delimiter, t_env *env)
{
	char	*line;
	int		i;

	while (1)
	{
		line = readline("heredoc>");
		if (!line)
			return (true);
		if (g_exit_codes == 130)
			return (false);
		if (ft_strncmp(line, delimiter, INT_MAX) == 0)
			break ;
		i = 0;
		while (line[i])
		{
			if (line[i] == '$' && line[i + 1])
				set_var_value(&line, env, i);
			if (line[i])
				i++;
		}
		write_in_heredoc(fd, line);
	}
	return (true);
}

int	set_heredoc(char *str, t_data *data)
{
	char	*delimiter;
	int		fd;
	t_env	*env;

	env = data->envlist;
	delimiter = str;
	fd = open("heredoc_tmp.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd == -1)
		return (fd);
	if (!get_line(fd, delimiter, env))
		data->cancel = true;
	close(fd);
	fd = open("heredoc_tmp.txt", O_RDONLY, 0644);
	if (fd > 0)
		unlink("heredoc_tmp.txt");
	return (fd);
}
