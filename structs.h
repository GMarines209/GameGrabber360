#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_BUFFER 256
#define MAX_PATH_SIZE 512
#define MAX_COMMAND 1024

#include <stdio.h>

typedef struct appConfig{
    char games_list_path[MAX_PATH_SIZE];
    char game_repo_360[MAX_PATH_SIZE];
    char game_repo_og[MAX_PATH_SIZE];
}appConfig;



typedef struct{
    int transfered;
    int skipped;
    int total_count;
    int go_again;
    long long total_size;
    FILE* skipped_games_log;
}appContext;

struct game_match
{
    char dir_name[MAX_PATH_SIZE];
    int lev_distance;
    float confidence;
};

typedef struct menu_selection{
    int game_source;
    int run_mode;
    char dest_Path[MAX_PATH_SIZE];
}selection;


#endif