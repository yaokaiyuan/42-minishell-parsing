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

#include "minishell.h"

// Function to tokenize input
char **tokenize_input(char *input) {
    char **tokens = malloc(1024 * sizeof(char *)); // Allocate memory for tokens
    char *token;
    int i = 0;

    token = strtok(input, " \n"); // Tokenize by space and newline
    while (token != NULL) {
        tokens[i++] = token;
        token = strtok(NULL, " \n");
    }
    tokens[i] = NULL; // Null-terminate the array
    return tokens;
}

// Function to free tokens
void free_tokens(char **tokens) {
    free(tokens); // Free the allocated memory for tokens
}

// Main parsing function
void parse_input(char *input) {
    char **tokens = tokenize_input(input); // Tokenize the input
    int index = 0;

    while (tokens[index]) {
        parse_command(tokens, &index); // Parse the command
        index++;
    }

    // Free tokens after parsing
    free_tokens(tokens);
}

// Function to handle quotes
void handle_quotes(char **tokens, int *index) {
    // Check for single quotes
    if (tokens[*index][0] == '\'') {
        // Skip until the next single quote
        while (tokens[++(*index)][0] != '\'' && tokens[*index] != NULL);
    }
    // Check for double quotes
    else if (tokens[*index][0] == '\"') {
        // Skip until the next double quote, allowing $ to be interpreted
        while (tokens[++(*index)][0] != '\"' && tokens[*index] != NULL) {
            if (tokens[*index][0] == '$') {
                // Handle variable expansion
                // For simplicity, we will just print a message
                printf("Variable expansion detected: %s\n", tokens[*index]);
            }
        }
    }
}

// Function to parse commands
void parse_command(char **tokens, int *index) {
    int start = *index; // Start index of the command

    // Handle quotes
    handle_quotes(tokens, index);

    // Execute the command
    execute_command(tokens, start, *index);
}

// Function to handle redirections
void parse_redirection(char **tokens, int *index) {
    if (strcmp(tokens[*index], "<") == 0) {
        // Handle input redirection
        (*index)++; // Move to the next token
        int fd = open(tokens[*index], O_RDONLY);
        if (fd < 0) {
            perror("Error opening file for input redirection");
            return;
        }
        dup2(fd, STDIN_FILENO); // Redirect input
        close(fd);
    } else if (strcmp(tokens[*index], ">") == 0) {
        // Handle output redirection
        (*index)++; // Move to the next token
        int fd = open(tokens[*index], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Error opening file for output redirection");
            return;
        }
        dup2(fd, STDOUT_FILENO); // Redirect output
        close(fd);
    } else if (strcmp(tokens[*index], "<<") == 0) {
        // Handle here-document
        (*index)++; // Move to the next token
        // Here you would implement the logic for here-documents
        printf("Here-document detected: %s\n", tokens[*index]);
    } else if (strcmp(tokens[*index], ">>") == 0) {
        // Handle output append
        (*index)++; // Move to the next token
        int fd = open(tokens[*index], O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
            perror("Error opening file for output append");
            return;
        }
        dup2(fd, STDOUT_FILENO); // Redirect output
        close(fd);
    }
}

// Function to handle pipes
void parse_pipe(char **tokens, int *index) {
    if (strcmp(tokens[*index], "|") == 0) {
        // Handle pipe
        printf("Pipe detected\n");
        (*index)++; // Move to the next token
        // Here you would implement the logic for piping between commands
    }
}

// Function to execute commands
void execute_command(char **tokens, int start, int end) {
    // Create a child process to execute the command
    pid_t pid = fork();
    if (pid == 0) {
        // In child process
        tokens[end] = NULL; // Null-terminate the command array
        if (execvp(tokens[start], &tokens[start]) == -1) {
            perror("Command execution failed");
        }
        exit(EXIT_FAILURE); // Exit child process if exec fails
    } else if (pid < 0) {
        perror("Fork failed");
    } else {
        // In parent process, wait for the child to finish
        wait(NULL);
    }
}

// Main function for testing
int main(int argc, char **argv) {
    char input[1024];

    printf("Minishell> ");
    while (fgets(input, sizeof(input), stdin)) {
        parse_input(input);
        printf("Minishell> ");
    }

    return 0;
}
