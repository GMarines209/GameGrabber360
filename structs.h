#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>

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
    char dir_name[256]; 
    int lev_distance;
    float confidence;
};

typedef struct menu_selection{
    int game_source;
    int run_mode;
    char dest_Path[256];
}selection;


#endif