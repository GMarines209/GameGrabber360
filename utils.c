#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h> 
#include "utils.h"

#define MIN(x, y, z) ((x) < (y) ? ((x) < (z) ? (x) : (z)) : ((y) < (z) ? (y) : (z)))
#define MAX_PATH_SIZE 512


void to_lower_string(char* str){
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

int get_type(const char* fileName){
    struct __stat64 path;

    if (_stat64(fileName, &path) != 0) {
        printf("\nStat failed for: %s\n", fileName); 
        perror("\nError was\n");                    
        return 0; 
    }

    if(S_ISDIR(path.st_mode) != 0){ //dir return 1
        return 1;
    }
    else if(S_ISREG(path.st_mode) != 0){ //file returns 2
        return 2;
    }
    else{
        return 0; // else returns 0
    }
}

long long get_dir_size(char* path){

    long long size = 0;
  
    struct __stat64 buffer;
    struct dirent *entry;

    //  base case - if path is a file not dir then return the size 
    if((get_type(path) == 2)){
        if(_stat64(path, &buffer) == 0) return buffer.st_size;
    }
    
    DIR* game_dir = opendir(path);
    if (game_dir == NULL) {
        printf("Could not open directory.\n");
        return 0;
    }  
    
    while((entry = readdir(game_dir)) != NULL){
        char*name = entry->d_name;
        if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0){
            continue;
        }
        char fullpath[MAX_PATH_SIZE];
        snprintf(fullpath,sizeof(fullpath),"%s\\%s",path,name);
        size += get_dir_size(fullpath);
    }
    
    closedir(game_dir);
    return size;
}

int Lev_Distance(char* str1, char* str2){
    
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    int *matrix = malloc((len1 + 1) * (len2 + 1) * sizeof(int));
    
    // Initialize matrix
    for (int i = 0; i <= len1; i++) {
        matrix[i * (len2 + 1)] = i;
    }
    for (int j = 0; j <= len2; j++) {
        matrix[j] = j;
    }

    // Calculate distances
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (str1[i - 1] == str2[j - 1]) ? 0 : 1;
            
            int delete_op = matrix[(i - 1) * (len2 + 1) + j] + 1;
            int insert_op = matrix[i * (len2 + 1) + (j - 1)] + 1;
            int sub_op    = matrix[(i - 1) * (len2 + 1) + (j - 1)] + cost;

            matrix[i * (len2 + 1) + j] = MIN(delete_op, insert_op, sub_op);
        }
    }

    int result = matrix[len1 * (len2 + 1) + len2];
    free(matrix);
    return result;


}

void print_splash_screen(){
    printf("       ______                     ______           __    __                 _____ _____ ____ \n");
    printf("      / ____/___ _____ ___  ___  / ____/________ _/ /_  / /_  ___  _____   |__  // ___// __ \\\n");
    printf("     / / __/ __ `/ __ `__ \\/ _ \\/ / __/ ___/ __ `/ __ \\/ __ \\/ _ \\/ ___/    /_ </ __ \\/ / / /\n");
    printf("    / /_/ / /_/ / / / / / /  __/ /_/ / /  / /_/ / /_/ / /_/ /  __/ /      ___/ / /_/ / /_/ / \n");
    printf("    \\____/\\__,_/_/ /_/ /_/\\___/\\____/_/   \\__,_/_.___/_.___/\\___/_/      /____/\\____/\\____/  \n");
    printf("======================================================================================================\n");
}



