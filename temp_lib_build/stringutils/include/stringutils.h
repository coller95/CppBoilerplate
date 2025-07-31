#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// String utility functions
void strutils_trim(char* str);
void strutils_reverse(char* str);
int strutils_count_words(const char* str);
void strutils_to_upper(char* str);
void strutils_to_lower(char* str);

#ifdef __cplusplus
}
#endif
