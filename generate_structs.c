/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_structs.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eusatiko <eusatiko@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 09:59:34 by eusatiko          #+#    #+#             */
/*   Updated: 2024/09/13 12:30:10 by eusatiko         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd *generate_structs(t_tok *head, int numargs, int numredir)
{
	t_cmd *cmd;
	int idx_a;
	int idx_r;
	int	err;

	err = 0;
	cmd = init_struct(numargs, numredir, &err);
	idx_a = 1;
	idx_r = 0;
	while (head->type != END && !err)
	{
		if (head->type == PIPE)
		{
			cmd->next = generate_structs(head->next, numargs, numredir);
			if (!cmd->next)
				cmd = free_cmd(cmd, 0);
			break ;
		}
		err = fill_struct(head, cmd, &idx_a, &idx_r);
		head = head->next;
	}
	if (err)
		cmd = free_cmd(cmd, 0);
	return (cmd);
}

t_cmd	*init_struct(int numargs, int numredir, int *err)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (cmd)
	{
		cmd->args = ft_calloc(numargs + 2, sizeof(char *));
		if (cmd->args)
		{
			cmd->redirect = ft_calloc(numredir + 1, sizeof(t_redirect));
			if (cmd->redirect)
				return (cmd);
		}
	}
	free_cmd(cmd, 0);
	*err = -1;
	return (NULL);
}

int	fill_struct(t_tok *head, t_cmd *cmd, int *idx_a, int *idx_r)
{
	if (head->type == CMD)
	{
		cmd->cmd = ft_strdup(head->word);
		if (!cmd->cmd)
			return (-1);
	}
	else if (head->type == ARGS)
	{
		cmd->args[*idx_a] = ft_strdup(head->word);
		if (!cmd->args[*idx_a])
			return (-1);
		*idx_a = *idx_a + 1;
	}
	else if (head->type >= HEREDOC)
	{
		cmd->redirect[*idx_r].type = head->type;
		cmd->redirect[*idx_r].value = ft_strdup(head->word);
		if (!cmd->redirect[*idx_r].value)
			return (-1);
		*idx_r = *idx_r + 1;
	}
	return (0);
}

t_cmd	*free_cmd(t_cmd *cmd, int i)
{
	if (!cmd)
		return (NULL);
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[++i])
			free(cmd->args[i]);
		if (cmd->args[0])
			free(cmd->args[0]);
		free(cmd->args);
	}
	if (cmd->redirect)
	{
		i = -1;
		while (cmd->redirect[++i].value)
		{
			if (cmd->redirect[i].type == HEREDOC)
				unlink(cmd->redirect[i].value);
			free(cmd->redirect[i].value);
		}
		free(cmd->redirect);
	}
	free(cmd);
	return (NULL);
}

void print_struct(t_cmd *cmd)
{
	if (!cmd)
		return ;
	printf("CMD: %s ARGS: ", cmd->cmd);
	int i = 0;
	while (cmd->args[++i])
		printf("%s ", cmd->args[i]);
	printf("%s\n", cmd->args[i]);
	i = -1;
	while (cmd->redirect[++i].value)
	{
		printf("redirect type is %d value is %s\n", cmd->redirect[i].type, cmd->redirect[i].value);
	}
}