/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykai-yua <ykai-yua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 14:20:15 by ykai-yua          #+#    #+#             */
/*   Updated: 2024/10/23 18:52:44 by ykai-yua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char* ft_strtok(char* str, const char* delim) {
    static char* current_position = NULL;
    char* token;
    int in_single_quote = 0;
    int in_double_quote = 0;

    if (str != NULL) {
        current_position = str;  // 设置当前解析的位置
    }

    if (current_position == NULL || *current_position == '\0') {
        return NULL;
    }

    while (*current_position && strchr(delim, *current_position) && !in_single_quote && !in_double_quote) {
        current_position++;
    }

    if (*current_position == '\0') {
        return NULL;
    }

    token = current_position;

    while (*current_position && (in_single_quote || in_double_quote || !strchr(delim, *current_position))) {
        if (*current_position == '\'') {
            in_single_quote = !in_single_quote;
        } else if (*current_position == '\"') {
            in_double_quote = !in_double_quote;
        }
        current_position++;
    }

    if (*current_position) {
        *current_position++ = '\0';
    }

    return token;
}
