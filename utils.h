#ifndef UTILS_H  
#define UTILS_H

#include <stddef.h>

void to_lower_string(char* str); 
int Lev_Distance(char* str1, char* str2);
long long get_dir_size(char* path);
int get_type(const char* fileName);
void print_splash_screen();
int num_policy(char* target, char* candidate);
int string_extraction(const char* str, char* nums, size_t capacity);


#endif