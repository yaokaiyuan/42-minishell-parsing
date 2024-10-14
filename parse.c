/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykai-yua <ykai-yua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/23 20:15:08 by ykai-yua          #+#    #+#             */
/*   Updated: 2024/10/05 09:17:12 by ykai-yua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Definition of a Binary Tree Node
typedef struct Node {
    void*         data;
    struct Node* left;
    struct Node* right;
} Node;


Node* parse_cmd(Node* node, char* token) {
    Node*   new_node = (Node*)malloc(sizeof(Node));
    new_node->data = token; // handle cmd call strtok to get the args
    new_node->left = NULL;
    new_node->right = NULL;
    if (node == NULL) {
        node = new_node;
    }
    else {
        node->right = new_node;
    }
    return node;
}

Node* parse_pipe(Node* node, char* token) {
    if (strcmp(token, "|") == 0) {
        Node*   new_node = (Node*)malloc(sizeof(Node));
        new_node->data = token; // handle pipe (set 0)
        new_node->left = node;
        new_node->right = NULL;
        return new_node;
    }
    else
    {
        return parse_cmd(node, token);
    }
}



void execute(Node* node) {
    if (node == NULL) {
        return;
    }
    execute(node->left);
    execute(node->right);
    printf("%s\n", (char *)node->data);
}


int main(int ac, char **av) {
    printf("%s\n", av[1]);
    Node *root = NULL;
    char *token = strtok(av[1], " ");
    while (token != NULL)
    {
        printf("token: %s\n", token);
        root = parse_pipe(root, token);
        token = strtok(NULL, " ");
    }
    execute(root);
    return 0;
}
