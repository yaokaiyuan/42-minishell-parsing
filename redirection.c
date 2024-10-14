/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykai-yua <ykai-yua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 17:13:27 by ykai-yua          #+#    #+#             */
/*   Updated: 2024/10/14 17:14:56 by ykai-yua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// Definition of a Binary Tree Node
typedef struct Node {
    char* command; // Command string
    char** args;   // Arguments array
    char* input_redirection; // Input redirection file
    char* output_redirection; // Output redirection file
    char* append_redirection; // Append redirection file
    char* here_doc_redirection; // Here document redirection file
    struct Node* left; // Left child (for pipes)
    struct Node* right; // Right child (for next command)
} Node;

// Function to create a new command node
Node* create_command_node(char* command, char** args, char* input_redirection, char* output_redirection, char* append_redirection, char* here_doc_redirection) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->command = strdup(command);
    new_node->args = args; // This should be dynamically allocated
    new_node->input_redirection = input_redirection ? strdup(input_redirection) : NULL;
    new_node->output_redirection = output_redirection ? strdup(output_redirection) : NULL;
    new_node->append_redirection = append_redirection ? strdup(append_redirection) : NULL;
    new_node->here_doc_redirection = here_doc_redirection ? strdup(here_doc_redirection) : NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Function to free a command node
void free_command_node(Node* node) {
    if (node) {
        free(node->command);
        if (node->args) {
            for (int i = 0; node->args[i] != NULL; i++) {
                free(node->args[i]);
            }
            free(node->args);
        }
        free(node->input_redirection);
        free(node->output_redirection);
        free(node->append_redirection);
        free(node->here_doc_redirection);
        free(node);
    }
}

// Function to parse command line input
Node* parse_command_line(char* input) {
    Node* head = NULL;
    Node* current = NULL;
    char* token = strtok(input, " ");
    char* command = NULL;
    char* input_redirection = NULL;
    char* output_redirection = NULL;
    char* append_redirection = NULL;
    char* here_doc_redirection = NULL;
    char** args = NULL;
    int arg_count = 0;

    while (token != NULL) {
        if (strcmp(token, "|") == 0) {
            // Create a command node for the current command
            if (command) {
                args[arg_count] = NULL; // Null-terminate the arguments
                Node* new_node = create_command_node(command, args, input_redirection, output_redirection, append_redirection, here_doc_redirection);
                if (head == NULL) {
                    head = new_node;
                } else {
                    current->right = new_node; // Link the new node
                }
                current = new_node; // Move current to the new node
                command = NULL; // Reset command
                input_redirection = NULL; // Reset input redirection
                output_redirection = NULL; // Reset output redirection
                append_redirection = NULL; // Reset append redirection
                here_doc_redirection = NULL; // Reset here document redirection
                arg_count = 0; // Reset argument count
                args = NULL; // Reset args
            }
        } else if (strcmp(token, ">") == 0) {
            // Handle output redirection
            output_redirection = strtok(NULL, " "); // Get the next token as the file name
        } else if (strcmp(token, ">>") == 0) {
            // Handle append redirection
            append_redirection = strtok(NULL, " "); // Get the next token as the file name
        } else if (strcmp(token, "<") == 0) {
            // Handle input redirection
            input_redirection = strtok(NULL, " "); // Get the next token as the file name
        } else if (strcmp(token, "<<") == 0) {
            // Handle here document redirection
            here_doc_redirection = strtok(NULL, " "); // Get the next token as the delimiter
        } else {
            // Collect command and arguments
            if (command == NULL) {
                command = strdup(token); // First token is the command
                args = malloc(10 * sizeof(char*)); // Allocate space for arguments
                arg_count = 0;
            } else {
                args[arg_count++] = strdup(token); // Add argument
            }
        }
        token = strtok(NULL, " ");
    }

    // Handle the last command
    if (command) {
        args[arg_count] = NULL; // Null-terminate the arguments
        Node* new_node = create_command_node(command, args, input_redirection, output_redirection, append_redirection, here_doc_redirection);
        if (head == NULL) {
            head = new_node;
        } else {
            current->right = new_node; // Link the new node
        }
    }

    return head;
}

// Function to execute the parsed commands (simplified)
void execute(Node* node) {
    while (node) {
        // Here you would implement the actual execution logic
        printf("Command: %s\n", node->command);
        if (node->input_redirection) {
            printf("Input Redirection: < to %s\n", node->input_redirection);
        }
        if (node->output_redirection) {
            printf("Output Redirection: > to %s\n", node->output_redirection);
        }
        if (node->append_redirection) {
            printf("Append Redirection: >> to %s\n", node->append_redirection);
        }
        if (node->here_doc_redirection) {
            printf("Here Document Redirection: << to %s\n", node->here_doc_redirection);
        }
        if (node->args) {
            printf("Arguments: ");
            for (int i = 0; node->args[i] != NULL; i++) {
                printf("%s ", node->args[i]);
            }
            printf("\n");
        }
        node = node->right; // Move to the next command
    }
}

int main(int ac, char **av) {
    if (ac > 1) {
        Node* commands = parse_command_line(av[1]);
        execute(commands);
        // Free the command nodes after execution
        // You would need to implement a function to free the entire linked list
    }
    return 0;
}
