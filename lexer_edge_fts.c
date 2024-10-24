/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_edge_fts.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eusatiko <eusatiko@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 10:40:04 by eusatiko          #+#    #+#             */
/*   Updated: 2024/10/16 11:38:13 by eusatiko         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_lex	*init_lex(char *input, t_tok *tail, t_data *data, int refr)
{
	static t_lex_state	state;
	t_lex				*lex;

	if (refr == 1)
		state = DELIM;
	lex = ft_calloc(1, sizeof(t_lex));
	if (!lex)
		return (NULL);
	lex->data = data;
	lex->tail = tail;
	lex->state = &state;
	lex->input = input;
	set_start(lex, ft_strlen(input));
	return (lex);
}

void	set_start(t_lex *lex, int len)
{
	if (!lex->tail)
	{
		lex->tail = gen_token(UNDETERM, len, lex);
		lex->head = lex->tail;
	}
	else
	{
		lex->head = lex->tail->next;
		if (lex->tail->type == PIPERR)
		{
			lex->tail->type = UNDETERM;
			free(lex->tail->word);
			lex->tail->word = ft_calloc(len + 1, sizeof(char));
			if (!lex->tail->word)
				lex->err = -1;
		}
		else
			extend_word(lex, len);
	}
}

t_tok	*gen_token(t_toktype type, int len, t_lex *lex)
{
	t_tok		*token;
	static int	input_len;

	if (!len)
		len = input_len;
	else
		input_len = len;
	token = ft_calloc(1, sizeof(t_tok));
	if (!token)
	{
		lex->err = -1;
		return (NULL);
	}
	token->type = type;
	if (type == END)
		token->word = ft_strdup("newline");
	else
		token->word = ft_calloc(len + 1, sizeof(char));
	if (!token->word)
	{
		free(token);
		lex->err = -1;
		return (NULL);
	}
	return (token);
}

void	extend_word(t_lex *lex, int len)
{
	int		oldlen;
	char	*newword;

	oldlen = ft_strlen(lex->tail->word);
	newword = ft_calloc(oldlen + len + 2, sizeof(char));
	if (!newword)
	{
		lex->err = -1;
		return ;
	}
	ft_strlcpy(newword, lex->tail->word, oldlen + 1);
	free(lex->tail->word);
	lex->tail->word = newword;
	lex->tail->word[lex->tail->idx++] = '\n';
}

t_tok	*set_end(t_lex *lex)
{
	if (*lex->state == INSQTS)
		lex->tail->type = SQERR;
	else if (*lex->state == INDQTS)
		lex->tail->type = DQERR;
	else if (lex->lastchar == '|' && !multi_pipe_check(lex->head))
		lex->tail->type = PIPERR;
	else if (*lex->state == DELIM || *lex->state == EXPAND)
	{
		lex->tail->type = END;
		free(lex->tail->word);
		lex->tail->word = ft_strdup("newline");
		if (!lex->tail->word)
			lex->err = -1;
	}
	else
	{
		*lex->state = DELIM;
		lex->tail->next = gen_token(END, 7, lex);
		if (!lex->err)
			lex->tail = lex->tail->next;
	}
	if (lex->err)
		return (free_tokens(lex->head));
	lex->tail->next = lex->head;
	return (lex->tail);
}
