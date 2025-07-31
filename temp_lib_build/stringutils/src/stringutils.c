#include "stringutils.h"
#include <ctype.h>
#include <string.h>

void strutils_trim(char* str) {
    if (!str)
        return;

    // Trim leading spaces
    char* start = str;
    while (isspace(*start)) start++;
    if (start != str)
        memmove(str, start, strlen(start) + 1);

    // Trim trailing spaces
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
}

void strutils_reverse(char* str) {
    if (!str)
        return;
        
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

int strutils_count_words(const char* str) {
    if (!str)
        return 0;

    int count = 0;
    int in_word = 0;

    while (*str) {
        if (isspace(*str)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        str++;
    }
    return count;
}

void strutils_to_upper(char* str) {
    if (!str)
        return;

    while (*str) {
        *str = toupper(*str);
        str++;
    }
}

void strutils_to_lower(char* str) {
    if (!str)
        return;

    while (*str) {
        *str = tolower(*str);
        str++;
    }
}
