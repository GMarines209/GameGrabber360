#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_BUFFER 256
#define MAX_PATH_SIZE 512
#define MAX_COMMAND 1024

#include <stdio.h>
#include <stdbool.h>

typedef struct appConfig{
    char games_list_path[MAX_PATH_SIZE];
    char game_repo_360[MAX_PATH_SIZE];
    char game_repo_og[MAX_PATH_SIZE];
    char dlc_path[MAX_PATH_SIZE];
    bool load_dlc;
}appConfig;

typedef struct{
    int transferred; //### num of games transferred
    int skipped; //### num of skipped games
    int total_count; //### num games in total
    int go_again; //### if user wants to loop program again
    long long total_size; //### total size of all games in bytes 
    long long bytes_transferred;//### running bytes counter 
    FILE* skipped_games_log;
}appContext;

struct game_match
{
    char dir_name[MAX_PATH_SIZE];
    int lev_distance;
    float confidence; //unused
};

typedef struct menu_selection{
    int game_source;
    int run_mode;
    char dest_Path[MAX_PATH_SIZE];
}selection;


#endif