/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inits.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: auspensk <auspensk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 12:16:52 by auspensk          #+#    #+#             */
/*   Updated: 2024/10/09 11:37:28 by auspensk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**dup_envp(char **envp)
{
	int		i;
	char	**dup_envp;

	i = 0;
	dup_envp = ft_calloc(100, sizeof(char *));
	if (!dup_envp)
		return (NULL);
	while (envp[i])
	{
		dup_envp[i] = ft_strdup(envp[i]);
		if (!dup_envp[i])
		{
			while (--i >= 0)
				free(dup_envp[i]);
			free(dup_envp);
			return (NULL);
		}
		i++;
	}
	return (dup_envp);
}

void	init_signals_quits(t_data *data)
{
	data->sa_quit = ft_calloc(1, sizeof(struct sigaction));
	if (!data->sa_quit)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
	data->sa_quit->sa_handler = SIG_IGN;
	data->sa_quit_child = ft_calloc(1, sizeof(struct sigaction));
	if (!data->sa_quit_child)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
	data->sa_quit_child->sa_handler = SIG_DFL;
	data->sa_quit_ex = ft_calloc(1, sizeof(struct sigaction));
	if (!data->sa_quit_ex)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
	data->sa_quit_ex->sa_handler = &handle_sig_ex;
	data->sa_quit_ex->sa_flags = SA_RESTART;
}

void	init_signals(t_data *data)
{
	data->sa = ft_calloc(1, sizeof(struct sigaction));
	if (!data->sa)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
	data->sa->sa_handler = &handle_sigint;
	data->sa->sa_flags = SA_RESTART;
	data->sa_ex = ft_calloc(1, sizeof(struct sigaction));
	if (!data->sa_ex)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
	data->sa_ex->sa_handler = &handle_sig_ex;
	data->sa_ex->sa_flags = SA_RESTART;
	data->sa_child = ft_calloc(1, sizeof(struct sigaction));
	if (!data->sa_child)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
	data->sa_child->sa_handler = SIG_DFL;
	init_signals_quits(data);
}

void	init_data(t_data *data, char **envp)
{
	data->pids = NULL;
	data->cmd = NULL;
	data->pids = NULL;
	data->envp = dup_envp(envp);
	data->paths = NULL;
	data->tty_in = ttyname(STDIN_FILENO);
	data->tty_out = ttyname(STDOUT_FILENO);
	data->st_code = 0;
	data->child = 0;
	data->std_in = dup(0);
	if (!data->envp)
		exit(clean_exit("failed to init data\n", EXIT_FAILURE, data));
}

int	new_pid(int pid, t_data *data)
{
	t_pids	*new_pid;
	t_pids	*cur_pid;

	new_pid = malloc(sizeof(t_pids));
	if (!new_pid)
		return (-1);
	new_pid->pid = pid;
	new_pid->next = NULL;
	if (data->pids)
	{
		cur_pid = data->pids;
		while (cur_pid->next)
			cur_pid = cur_pid->next;
		cur_pid->next = new_pid;
	}
	else
		data->pids = new_pid;
	return (0);
}
