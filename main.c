/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eusatiko <eusatiko@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/21 15:07:50 by auspensk          #+#    #+#             */
/*   Updated: 2024/09/23 12:07:33 by eusatiko         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
NON-INTERACTIVE MAIN

int	main(int argc, char *argv[], char *envp[])
{
	t_data	data;
	t_tok *head;
	t_tok *tail;

	if (argc > 2)
	{
		printf ("no arguments required, only program name: %s\n", argv[0]);
		return (1);
	}
	init_data(&data, envp);
	tail = lexer(input, NULL, data);
	head = tail->next;
	tail->next = NULL;
	data.cmd = parser(head, &data);
	if (data.cmd)
		execute_loop(&data);
	exit(clean_exit(NULL, EXIT_SUCCESS, &data));
}
*/

int lastsignal;

void	handle_sigint(int sig)
{
	lastsignal = sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	//rl_clear_display();
	
}

void	handle_sigint_ex(int sig)
{
	lastsignal = sig;
	printf("\n");
}

/*
int g_signal_code = 0;

void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_code = 1;
	//rl_on_new_line();
	// if (exec_chld)
	// 	write(1, "\n", 1);
	// else
	// 	write(1, "\nminishell> ", ft_strlen("\nminishell> "));
	// rl_replace_line("", 0);
	// rl_redisplay();
	// write(1, "\nminishell> ", ft_strlen("\nminishell> "));
	// fflush(stdout); //not allowed function, but was advised in codevault
}

void	handle_sigint_ch(int sig)
{
	(void)sig;
}

*/

int	main(int argc, char *argv[], char *envp[])
{
	t_data	data;
	t_tok	*head;

	(void)argv;
	init_signals(&data);
	sigaction(SIGQUIT, data.sa_quit, NULL);
	sigaction(SIGINT, data.sa, NULL);
	if (argc > 1)
	{
		ft_putstr_fd ("no arguments required, only program name\n", 2);
		exit(EXIT_FAILURE);
	}
	init_data(&data, envp);
	while (1)
	{
		//printf("in while loop\n");
		lastsignal = 0;
		head = read_input(&data);
		if (!head) //ctrl + D
		{
			data.cmd = NULL;
			break ;
		}
		if (head->type == END)
		{
			free_tokens(head);
			continue ;
		}
		else
			data.cmd = parser(head, &data);
		if (!data.cmd)
			continue ;
		sigaction(SIGINT, data.sa_ex, data.sa);
		execute_loop(&data, data.sa_child);
		free_cmds(data.cmd);
	}
	printf("exit\n");
	exit(clean_exit(NULL, EXIT_SUCCESS, &data)); //when should main return sth other than 0?
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
	while (tail && tail->type >= SQERR && tail->type <= PIPERR)
	{
		free(input);
		input = readline("> ");
		if (input == NULL) //will happen with ctrl+D
			tail = free_tokens(tail->next);
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
