/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykai-yua <ykai-yua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/05 09:04:59 by ykai-yua          #+#    #+#             */
/*   Updated: 2024/10/05 09:12:45 by ykai-yua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

// Function prototypes
void parse_input(char *input);
void parse_command(char **tokens, int *index);
void parse_redirection(char **tokens, int *index);
void parse_pipe(char **tokens, int *index);
void handle_quotes(char **tokens, int *index);
char **tokenize_input(char *input);
void free_tokens(char **tokens);
void execute_command(char **tokens, int start, int end);

#endif // MINISHELL_H
