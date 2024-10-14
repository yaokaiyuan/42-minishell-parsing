#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* ft_strtok(char* str, const char* delimiters) {
    static char* last = NULL; // 保存上次调用的状态
    char* current;

    // 如果 str 不是 NULL，说明是第一次调用
    if (str != NULL) {
        last = str; // 初始化 last
    }

    // 如果 last 为空，说明没有更多的标记
    if (last == NULL) {
        return NULL;
    }

    // 跳过前导分隔符
    while (*last && strchr(delimiters, *last)) {
        last++;
    }

    // 如果到达字符串末尾，返回 NULL
    if (*last == '\0') {
        last = NULL;
        return NULL;
    }

    // 找到标记的结束位置
    current = last;
    while (*current && !strchr(delimiters, *current)) {
        current++;
    }

    // 如果找到标记的结束位置，替换为 '\0'，并更新 last
    if (*current) {
        *current = '\0';
        last = current + 1;
    } else {
        last = NULL; // 到达字符串末尾
    }

    return str; // 返回当前标记
}

// 主函数示例
int main() {
    char input[] = "Hello, world! This is a test.";
    const char* delimiters = " ,.!"; // 定义分隔符
    char* token;

    // 使用 ft_strtok 进行分词
    token = ft_strtok(input, delimiters);
    while (token != NULL) {
        printf("token: %s\n", token);
        token = ft_strtok(NULL, delimiters);
    }

    return 0;
}