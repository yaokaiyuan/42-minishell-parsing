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
#include <fcntl.h>

// Definition of a Binary Tree Node
typedef struct Node {
    void*         data; // Command and arguments
    struct Node* left;
    struct Node* right;
} Node;

// Function to handle redirection
char* handle_redirection(char* command, char** redirection_file) {
    char* cmd = strdup(command); // 复制命令以避免修改原始字符串
    char* token;

    // 处理输出重定向 >
    if ((token = strstr(cmd, ">")) != NULL) {
        *token = '\0'; // 将 > 替换为字符串结束符
        *redirection_file = strdup(token + 1); // 获取文件名
    }
    // 处理追加输出重定向 >>
    else if ((token = strstr(cmd, ">>")) != NULL) {
        *token = '\0'; // 将 >> 替换为字符串结束符
        *redirection_file = strdup(token + 2); // 获取文件名
    }
    // 处理输入重定向 <
    else if ((token = strstr(cmd, "<")) != NULL) {
        *token = '\0'; // 将 < 替换为字符串结束符
        *redirection_file = strdup(token + 1); // 获取文件名
    }
    // 处理 Here Document <<
    else if ((token = strstr(cmd, "<<")) != NULL) {
        *token = '\0'; // 将 << 替换为字符串结束符
        *redirection_file = strdup(token + 2); // 获取分隔符
        printf("Here Document delimiter: %s\n", *redirection_file);
    }

    return cmd; // 返回处理后的命令
}

// Function to handle command execution
void execute_command(Node* node) {
    if (node == NULL) {
        return;
    }

    // 处理左子树（命令）
    execute_command(node->left);

    // 处理当前节点（命令或重定向）
    char* command = (char*)node->data;
    char* redirection_file = NULL;
    char* processed_command = handle_redirection(command, &redirection_file); // 传递重定向文件指针

    // 解析命令和参数
    char* token = strtok(processed_command, " ");
    char* args[100]; // 假设最多有 100 个参数
    int arg_count = 0;

    // 将第一个参数作为命令
    args[arg_count++] = strdup(token); // 第一个 token 是命令

    // 继续解析后续参数
    token = strtok(NULL, " ");
    while (token != NULL) {
        // 如果是重定向文件，跳过
        if (redirection_file != NULL && strcmp(token, redirection_file) == 0) {
            token = strtok(NULL, " "); // 跳过重定向文件
            continue;
        }
        // 将参数添加到参数数组中
        args[arg_count++] = strdup(token);
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // 以 NULL 结束参数数组

    // 打印重定向文件名
    if (redirection_file != NULL) {
        printf("Redirection file: %s\n", redirection_file);
        free(redirection_file); // 释放重定向文件名
        printf("Redirection executed successfully.\n"); // 打印重定向执行成功信息
    } else {
        printf("No redirection found.\n"); // 打印未找到重定向信息
    }

    // 执行命令
    printf("Executing command: %s\n", processed_command);
    printf("With arguments: ");
    for (int i = 0; i < arg_count; i++) {
        printf("%s ", args[i]); // 打印每个参数
    }
    printf("\n");

    // 这里可以使用 execvp 或其他函数来执行命令
    // 例如: execvp(args[0], args);

    // 释放参数数组内存
    for (int i = 0; i < arg_count; i++) {
        free(args[i]);
    }
    free(processed_command); // 释放处理后的命令内存

    // 处理右子树（下一个命令）
    execute_command(node->right);
}

// Function to create a new command node
Node* create_command_node(char* command) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = strdup(command); // 复制命令
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Function to parse command and its arguments
Node* parse_command_with_args(Node* node, char* input) {
    char* command = NULL;
    char* token = strtok(input, " ");
    char* redirection_file = NULL;

    while (token != NULL) {
        // 检查是否为重定向符号
        if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0 || strcmp(token, "<") == 0 || strcmp(token, "<<") == 0) {
            // 将当前命令添加到树中
            if (command != NULL) {
                node = create_command_node(command); // 创建命令节点
                free(command); // 释放旧的命令字符串
                command = NULL; // 重置命令
            }
            // 处理重定向符号
            // 这里可以将重定向符号与命令一起存储
        } else {
            // 处理命令和参数
            if (command == NULL) {
                command = strdup(token); // 第一个 token 是命令
            } else {
                // 将参数添加到当前命令字符串中
                char* new_command = malloc(strlen(command) + strlen(token) + 2); // +2 for space and null terminator
                sprintf(new_command, "%s %s", command, token);
                free(command); // 释放旧的命令字符串
                command = new_command; // 更新命令
            }
        }
        token = strtok(NULL, " ");
    }

    // 处理最后一个命令
    if (command != NULL) {
        node = create_command_node(command); // 创建命令节点
        free(command); // 释放命令字符串
    }

    // 处理重定向文件
    if (redirection_file != NULL) {
        printf("Redirection file: %s\n", redirection_file);
        free(redirection_file); // 释放重定向文件名
    }

    return node;
}

Node* parse_cmd(Node* node, char* token) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = strdup(token); // 复制 token 以避免修改原始字符串
    new_node->left = NULL;
    new_node->right = NULL;
    if (node == NULL) {
        node = new_node;
    } else {
        node->right = new_node;
    }
    return node;
}

Node* parse_pipe(Node* node, char* token) {
    if (strcmp(token, "|") == 0) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->data = strdup(token); // 复制 token 以避免修改原始字符串
        new_node->left = node;
        new_node->right = NULL;
        return new_node;
    } else {
        return parse_cmd(node, token);
    }
}

int main(int ac, char **av) {
    if (ac < 2) {
        fprintf(stderr, "Usage: %s <command_line>\n", av[0]);
        return EXIT_FAILURE;
    }

    Node *root = NULL;
    char *input = strdup(av[1]); // 复制输入以避免修改原始字符串
    char *token = strtok(input, " ");

    while (token != NULL) {
        root = parse_pipe(root, token); // 解析命令和管道
        token = strtok(NULL, " ");
    }

    execute_command(root); // 执行命令
    free(input); // 释放输入字符串的内存
    return 0;
}