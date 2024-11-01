/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykai-yua <ykai-yua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/05 09:04:59 by ykai-yua          #+#    #+#             */
/*   Updated: 2024/10/23 18:54:30 by ykai-yua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef enum {
    COMMAND,
    IN,      // '<' Input redirection
    OUT,     // '>' Output redirection
    APPEND,  // '>>' Append redirection
    PIPE,
    ARGUMENT // Argument type
} TokenType;

typedef struct ParamNode {
    char* data;
    struct ParamNode* next;
} ParamNode;

typedef struct Node {
    char* data;
    TokenType type;
    ParamNode* params;
    char* infile;        // Input redirection target
    char* outfile;       // Output redirection target
    char* appendfile;    // Append redirection target
    struct Node* left;
    struct Node* right;
} Node;

Node* create_node(char* token, TokenType type);
ParamNode* create_param_node(char* data);
void handle_error(const char* msg);
Node* parse_redirect(Node* cmd_node, char* token, char** next_token);
Node* parse_pipe(Node* left_cmd, char* token);
Node* parse_cmd(Node* left_cmd, char* token);
void execute(Node* node);
char* ft_strtok(char* str, const char* delim);

#endif // MINISHELL_H
