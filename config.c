#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "config.h"
#include "structs.h"
#include "file_browser.h"


appConfig load_settings() {
    //open config file
    FILE *config = fopen("settings.ini","r");

    if (config == NULL){
        return first_time_setup();
    }

    appConfig loaded_config = {0};

    char buffer[MAX_BUFFER];
    char curr_selection[MAX_PATH_SIZE];
    
    while (fgets(buffer, sizeof(buffer), config) != NULL) {

        // trash it if its a comment etc
        if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == ';') {
            continue; 
        } 
        //if its the start and end of a tag save it
        else if(buffer[0] == '['){
            char* end_bracket = strchr(buffer, ']');

            if(end_bracket != NULL){
                *end_bracket = '\0';
                strcpy(curr_selection, buffer + 1);
                continue;
            }
        }
        else{
            char* equals = strchr(buffer,'=');
            if(equals != NULL){
                *equals = '\0';
                char* new_line = strchr(equals+1,'\n');
                if(new_line != NULL){
                    *new_line = '\0';
                }

                if(strcmp(buffer,"Path360") == 0){
                    strncpy(loaded_config.game_repo_360,equals+1,MAX_PATH_SIZE -1);
                }
                else if(strcmp(buffer,"PathOG") == 0){
                    strncpy(loaded_config.game_repo_og,equals+1,MAX_PATH_SIZE -1);
                }
                else {
                    strncpy(loaded_config.games_list_path,equals+1,MAX_PATH_SIZE -1);
                }
                
                

            }
        }

    }

    fclose(config);
    return loaded_config;
    
}

appConfig first_time_setup(){

    appConfig config = {0};

    printf("\nIt looks like you're missing your config file.\n");
    printf("Please continue to first time setup:\n\n");

    printf("Select your Xbox360 game repository:");
    char* selected_path = PickFolder(NULL);
    while(selected_path == NULL){
        printf("\nSelection cancelled. Try Again...\n");
        selected_path = PickFolder(NULL);
    }
    // Copy it into struct safely
    strncpy(config.game_repo_360, selected_path, MAX_PATH_SIZE -1);
    config.game_repo_360[MAX_PATH_SIZE -1] = '\0'; 
    free(selected_path);

    printf("\nSelect your Original Xbox game repository:");
    selected_path = PickFolder(NULL);
    while(selected_path == NULL){
        printf("\nSelection cancelled. Try Again...\n");
        selected_path = PickFolder(NULL);
    }
    // Copy it into struct safely
    strncpy(config.game_repo_og, selected_path, MAX_PATH_SIZE -1);
    config.game_repo_og[MAX_PATH_SIZE -1] = '\0'; 
    free(selected_path);

    printf("\nSelect your games list:");
    selected_path = PickFile(NULL);
    while(selected_path == NULL){
        printf("\nSelection cancelled. Try Again...\n");
        selected_path = PickFile(NULL);
    }
    // Copy it into struct safely
    strncpy(config.games_list_path, selected_path, MAX_PATH_SIZE -1);
    config.games_list_path[MAX_PATH_SIZE -1] = '\0'; 
    free(selected_path);

    FILE *save_file = fopen("settings.ini", "w");

    if (save_file != NULL) {
        // 2. Print exactly the format your parser expects
        fprintf(save_file, "[Directories]\n");
        fprintf(save_file, "Path360=%s\n", config.game_repo_360);
        fprintf(save_file, "PathOG=%s\n", config.game_repo_og);
        fprintf(save_file, "ClientList=%s\n", config.games_list_path);
        
        // 3. Flush to disk and close
        fclose(save_file);
        printf("\n[SUCCESS] Configuration saved to settings.ini\n");
    } else {
        printf("\n[ERROR] Failed to write settings.ini to disk.\n");
    }

    return config;
}
