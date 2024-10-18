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

typedef struct Node {
    void* data;
    struct Node* left;
    struct Node* right;
} Node;

Node* create_node(char* token);
void handle_error(const char* msg);
Node* parse_redirect(Node* cmd_node, char* token, char** next_token);
Node* parse_pipe(Node* left_cmd);
Node* parse_cmd(Node* cmd_node, char* token);
Node* create_cmd_tree(char* token);
void print_tree(Node* node);
char* ft_strtok(char* str, const char* delim);
char* get_variable_value(const char* var_name);
char* process_double_quotes(char* str);


#endif // MINISHELL_H
