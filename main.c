/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: auspensk <auspensk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 15:07:50 by auspensk          #+#    #+#             */
/*   Updated: 2024/10/09 10:27:54 by auspensk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t lastsignal;

void	main_loop(t_data *data, t_tok **head)
{
	while (1)
	{
		//printf("begin of while, sig is %d\n", lastsignal);
		sigaction(SIGQUIT, data->sa_quit, NULL);
		sigaction(SIGINT, data->sa, NULL);
		if (lastsignal)
			printf("\n");
		lastsignal = 0;
		*head = read_input(data);
		if (!*head) //ctrl + D
		{
			data->cmd = NULL;
			break ;
		}
		if ((*head)->type == END)
		{
			free_tokens(*head);
			continue ;
		}
		else
			data->cmd = parser(*head, data);
		if (!data->cmd)
			continue ;
		sigaction(SIGINT, data->sa_ex, NULL);
		sigaction(SIGQUIT, data->sa_quit_ex, NULL);
		//printf("lastsignal is %d\n", lastsignal);
		execute_loop(data);
		free_cmds(data->cmd);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	t_data	data;
	t_tok	*head;

	(void)argv;
	//if (!isatty(0) || !isatty(1))
	//	return (clean_exit("piping ./execs is not supported\n", errno, NULL));
	lastsignal = 0;
	init_signals(&data);
	// sigaction(SIGQUIT, data.sa_quit, NULL);
	// sigaction(SIGINT, data.sa, NULL);
	if (argc > 1)
	{
		ft_putstr_fd ("no arguments required, only program name\n", 2);
		exit(EXIT_FAILURE);
	}
	init_data(&data, envp);
	main_loop(&data, &head);
	printf("exit\n");
	exit(clean_exit(NULL, data.st_code, &data)); //when should main return sth other than 0?
}

t_tok	*read_input(t_data *data)
{
	char	*input;
	t_tok	*tail;
	t_tok	*head;

	input = readline("minishell> "); //
	if (input == NULL) //will happen with ctrl+D
		return (NULL);
	if (*input)
		add_history(input);
	tail = lexer(input, NULL, data);
	lastsignal = 0;
	while (tail && tail->type >= SQERR && tail->type <= PIPERR)
	{
		free(input);
		input = readline("> ");
		if (input == NULL) //will happen with ctrl+D
			return(free_tokens(tail->next));
		tail = lexer(input, tail, data);
	}
	if (input)
		free(input);
	if (!tail)
		return (NULL);
	head = tail->next;
	tail->next = NULL;
	return (head);
}
