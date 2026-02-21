#ifndef UTILS_H  
#define UTILS_H
#define MIN(x, y, z) ((x) < (y) ? ((x) < (z) ? (x) : (z)) : ((y) < (z) ? (y) : (z)))

void toLowerString(char* str);
int Lev_Distance(char* str1, char* str2);
long long get_dir_size(char* path);
int getType(const char* fileName);


#endif