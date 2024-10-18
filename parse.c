#include "minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_variable_value(const char* var_name) {
    return getenv(var_name);
}

char* process_double_quotes(char* str) {
    size_t length = strlen(str);
    char* result = malloc(length * 2);  // 预留足够空间以应对环境变量替换
    char* result_ptr = result;
    const char* current_ptr = str;

    while (*current_ptr) {
        if (*current_ptr == '$') {
            current_ptr++;
            char var_name[256];
            int i = 0;

            // 获取变量名
            while (*current_ptr && (*current_ptr != ' ' && *current_ptr != '$' && *current_ptr != '\"')) {
                var_name[i++] = *current_ptr++;
            }
            var_name[i] = '\0';  // 结束变量名字符串

            // 获取环境变量值
            char* var_value = get_variable_value(var_name);
            if (var_value) {
                // 将环境变量值复制到结果中
                strcpy(result_ptr, var_value);
                result_ptr += strlen(var_value);
            }
        } else {
            // 直接复制字符（包括空格）
            *result_ptr++ = *current_ptr++;
        }
    }
    *result_ptr = '\0';  // 结束结果字符串
    return result;
}

// 创建一个新节点
Node* create_node(char* token) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = token;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// 错误处理函数
void handle_error(const char* msg) {
    printf("Error: %s\n", msg);
    exit(1);
}

// 解析重定向符号并检查错误
Node* parse_redirect(Node* cmd_node, char* token, char** next_token) {
    if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0) {
        *next_token = ft_strtok(NULL, " ");
        if (*next_token == NULL) {
            handle_error("Missing file after redirection.");
        }
        // 创建重定向节点
        Node* redirect_node = create_node(token);
        redirect_node->left = cmd_node;
        Node* file_node = create_node(*next_token);
        redirect_node->right = file_node;
        return redirect_node;
    }
    return cmd_node;
}

// 解析管道符号并检查错误
Node* parse_pipe(Node* left_cmd) {
    Node* pipe_node = create_node("|");
    pipe_node->left = left_cmd;

    // 继续解析管道后的命令
    char* next_token = ft_strtok(NULL, " ");
    if (next_token == NULL) {
        handle_error("Missing command after pipe.");
    }

    // 递归处理管道后的命令
    pipe_node->right = create_cmd_tree(next_token);  // 递归解析管道后的命令
    return pipe_node;
}

// 解析命令和参数
Node* parse_cmd(Node* cmd_node, char* token) {
    if (cmd_node == NULL) {
        return create_node(token);  // 创建命令节点
    } else {
        // 将参数作为命令的右子树
        Node* param_node = create_node(token);
        Node* temp = cmd_node;
        while (temp->right != NULL) {
            temp = temp->right;
        }
        temp->right = param_node;
        return cmd_node;
    }
}

Node* create_cmd_tree(char* token) {
    Node* root = NULL;
    Node* current_cmd = NULL;
    char* next_token = NULL;
    int single_quote_count = 0;
    int double_quote_count = 0;

    while (token != NULL) {
        // 统计单引号和双引号的数量
        for (char* p = token; *p != '\0'; p++) {
            if (*p == '\'') {
                single_quote_count++;
            } else if (*p == '\"') {
                double_quote_count++;
            }
        }

        // 处理管道符号
        if (strcmp(token, "|") == 0) {
            root = parse_pipe(current_cmd);  // 解析当前命令并处理管道
            current_cmd = NULL;  // 管道后开始新命令
        }
        // 处理重定向符号
        else if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0) {
            current_cmd = parse_redirect(current_cmd, token, &next_token);
            token = next_token;  // 跳过文件名
        }
        // 处理普通命令或参数
        else {
            // 处理双引号内容
            if (strchr(token, '\"')) {
                char* processed_token = process_double_quotes(token);
                current_cmd = parse_cmd(current_cmd, processed_token);  // 解析命令和参数
                free(processed_token);  // 释放处理过的token
            } else {
                current_cmd = parse_cmd(current_cmd, token);  // 解析命令和参数
            }

            if (root == NULL) {
                root = current_cmd;  // 第一个命令为根节点
            }
        }

        token = ft_strtok(NULL, " ");
    }

    // 检查单引号和双引号是否成对出现
    if (single_quote_count % 2 != 0) {
        handle_error("Unmatched single quotes in command line.");
    }
    if (double_quote_count % 2 != 0) {
        handle_error("Unmatched double quotes in command line.");
    }

    return root;
}

// 打印二叉树（遵循 Bash 的执行顺序）
void print_tree(Node* node) {
    if (node == NULL) {
        return;
    }
    
    // 对于管道操作，先打印左侧的命令，再打印右侧
    if (strcmp((char*)node->data, "|") == 0) {
        print_tree(node->left);
        printf("| ");
        print_tree(node->right);
    }
    // 对于重定向操作，打印时应保证左子树先于右子树
    else if (strcmp((char*)node->data, "<") == 0 || strcmp((char*)node->data, ">") == 0) {
        print_tree(node->left);
        printf("%s ", (char*)node->data);  // 打印重定向符号
        print_tree(node->right);  // 打印重定向文件
    }
    // 对于普通命令和参数，直接按左右子树递归打印
    else {
        printf("%s ", (char*)node->data);
        print_tree(node->right);  // 打印命令参数
    }
}

int main(int ac, char** av) {
    if (ac < 2) {
        printf("Usage: %s \"command\"\n", av[0]);
        return 1;
    }

    // 拼接命令行参数到一个字符串中
    size_t buffer_size = 0;
    for (int i = 1; i < ac; i++) {
        buffer_size += strlen(av[i]) + 1;  // +1 for space or null terminator
    }

    char* command = (char*)malloc(buffer_size);
    command[0] = '\0';  // 初始化为空字符串
    for (int i = 1; i < ac; i++) {
        strcat(command, av[i]);
        if (i < ac - 1) {
            strcat(command, " ");  // 在参数之间添加空格
        }
    }

    char* token = ft_strtok(command, " ");
    Node* root = create_cmd_tree(token);

    // 打印执行顺序
    printf("Execution order:\n");
    print_tree(root);
    printf("\n");

    free(command);  // 释放命令字符串的内存

    return 0;
}
