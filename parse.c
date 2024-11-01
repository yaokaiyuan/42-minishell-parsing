#include "minishell.h"

// Create a new node with a specified token type
Node* create_node(char* token, TokenType type) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        handle_error("Memory allocation failed for Node.");
    }
    new_node->data = strdup(token);
    new_node->type = type;
    new_node->params = NULL;
    new_node->infile = NULL;
    new_node->outfile = NULL;
    new_node->appendfile = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Create a parameter node
ParamNode* create_param_node(char* data) {
    ParamNode* new_param = (ParamNode*)malloc(sizeof(ParamNode));
    if (new_param == NULL) {
        handle_error("Memory allocation failed for ParamNode.");
    }
    new_param->data = strdup(data);
    new_param->next = NULL;
    return new_param;
}

// Error handling function
void handle_error(const char* msg) {
    printf("Error: %s\n", msg);
    exit(1);
}

// Parse redirection symbols and store target files
Node* parse_redirect(Node* cmd_node, char* token, char** next_token) {
    *next_token = ft_strtok(NULL, " ");
    if (*next_token == NULL) {
        handle_error("Missing file after redirection.");
    }

    if (strcmp(token, "<") == 0) {
        cmd_node->infile = strdup(*next_token);
    } else if (strcmp(token, ">") == 0) {
        cmd_node->outfile = strdup(*next_token);
    } else if (strcmp(token, ">>") == 0) {
        cmd_node->appendfile = strdup(*next_token);
    }

    return cmd_node; // Return the command node with updated redirection
}

// Parse the pipe symbol
Node* parse_pipe(Node* left_cmd, char* token) {
    if (strcmp(token, "|") == 0) {
        Node* pipe_node = create_node("|", PIPE);
        pipe_node->left = left_cmd;

        char* next_token = ft_strtok(NULL, " ");
        if (next_token == NULL) {
            handle_error("Missing command after pipe.");
        }

        pipe_node->right = parse_cmd(NULL, next_token); // Recursively parse the command after the pipe
        return pipe_node;
    } else {
        return parse_cmd(left_cmd, token); // Handle command and arguments
    }
}

// Parse command, arguments, and redirection within a single command line
Node* parse_cmd(Node* cmd_node, char* token) {
    if (cmd_node == NULL) {
        cmd_node = create_node(token, COMMAND); // Initialize command node
    } else if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
        // Handle redirection symbols
        char* next_token;
        cmd_node = parse_redirect(cmd_node, token, &next_token);
        token = next_token; // Move to next token after file redirection
    } else {
        // Add as an argument
        ParamNode* param_node = create_param_node(token);
        if (cmd_node->params == NULL) {
            cmd_node->params = param_node; // First argument
        } else {
            ParamNode* temp = cmd_node->params;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = param_node; // Add new parameter
        }
    }

    return cmd_node; // Return updated command node
}

// Execute command tree
void execute(Node* node) {
    if (node == NULL) {
        return;
    }

    execute(node->left); // Execute left side first

    // Print current command or operation
    printf("Executing: %s (%s)\n", node->data,
           node->type == IN ? "IN" :
           node->type == OUT ? "OUT" :
           node->type == APPEND ? "APPEND" :
           node->type == PIPE ? "PIPE" : "COMMAND");

    // Print redirection files if any
    if (node->infile) {
        printf("Input file: %s\n", node->infile);
    }
    if (node->outfile) {
        printf("Output file: %s\n", node->outfile);
    }
    if (node->appendfile) {
        printf("Append file: %s\n", node->appendfile);
    }

    // Print arguments if any
    if (node->params != NULL) {
        ParamNode* param = node->params;
        printf("Arguments: ");
        while (param != NULL) {
            printf("%s ", param->data);
            param = param->next;
        }
        printf("\n");
    }

    execute(node->right); // Then execute right side
}

int main() {
    char* input;

    while (1) {
        input = readline("./a.out ");
        if (input == NULL) {
            printf("\n");
            break;
        }

        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        add_history(input);

        char* token = ft_strtok(input, " ");
        Node* root = NULL;

        while (token != NULL) {
            root = parse_pipe(root, token); // Recursively parse
            token = ft_strtok(NULL, " ");
        }

        execute(root); // Execute parsed command tree

        free(input);
        // Here you can add code to free the command tree
    }

    return 0;
}
