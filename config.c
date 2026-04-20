#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include "utils.h"
#include "config.h"
#include "structs.h"
#include "file_browser.h"


appConfig load_settings(){
    //open config file
    FILE *config = fopen("settings.ini","r");

    if (config == NULL){
        return first_time_setup();
    }

    appConfig loaded_config = {0};

    char buffer[MAX_BUFFER];
    char curr_selection[MAX_PATH_SIZE] = "";
    
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
                strcpy(curr_selection, buffer + 1); //save tag as current selection
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
                
                if(strcmp(curr_selection,"Directories") == 0){
                    if(strcmp(buffer,"Path360") == 0){
                        strncpy(loaded_config.game_repo_360,equals+1,MAX_PATH_SIZE -1);
                    }
                    else if(strcmp(buffer,"PathOG") == 0){
                        strncpy(loaded_config.game_repo_og,equals+1,MAX_PATH_SIZE -1);
                    }
                    else if(strcmp(buffer,"GAMESLIST") == 0) {
                        strncpy(loaded_config.games_list_path,equals+1,MAX_PATH_SIZE -1);
                    }
                    else if(strcmp(buffer,"PathDLC") == 0){
                        strncpy(loaded_config.dlc_path,equals+1,MAX_PATH_SIZE -1);
                    }
                }

                if(strcmp(curr_selection,"Preferences") == 0){
                    if(strcmp(buffer,"AlwaysDownloadDlC") == 0){
                        loaded_config.load_dlc = atoi(equals + 1);
                    }
                }
                
            }
        }

    }

    fclose(config);
    return loaded_config;
    
}

void prompt_for_path(const char* prompt_text, char* destination_buffer, bool is_file){
    
    printf("%s",prompt_text);
    if(is_file == false){
        char* selected_path = PickFolder(NULL);
        while(selected_path == NULL){
            printf("\nSelection cancelled. Try Again...\n");
            selected_path = PickFolder(NULL);
        }
        strncpy(destination_buffer, selected_path, MAX_PATH_SIZE -1);
        destination_buffer[MAX_PATH_SIZE -1] = '\0'; 
        free(selected_path);
        
    }
    else{
        char* selected_path = PickFile(NULL);
        while(selected_path == NULL){
            printf("\nSelection cancelled. Try Again...\n");
            selected_path = PickFile(NULL);
        }
        strncpy(destination_buffer, selected_path, MAX_PATH_SIZE -1);
        destination_buffer[MAX_PATH_SIZE -1] = '\0'; 
        free(selected_path);
    }

}

appConfig first_time_setup(){

    appConfig config = {0};

    printf("\nIt looks like you're missing your config file.\n");
    printf("Please continue to first time setup:\n\n");

    prompt_for_path("\nSelect your Xbox360 game repository:", config.game_repo_360, false);
    prompt_for_path("\nSelect your Original Xbox game repository:", config.game_repo_og, false);
    prompt_for_path("\nSelect your games list:", config.games_list_path, true);
    prompt_for_path("\nSelect your DLC repository:", config.dlc_path, false);

    int dlc;
    printf("\nDo you want to always download DLC when available? ([1]Yes/[0]No) ");
    scanf("%d",&dlc);
    while(dlc != 0 && dlc != 1 ){
        printf("\nInvalid selection. Try again\n");
        scanf("%d",&dlc);
        while ((getchar()) != '\n');
    }


    FILE *save_file = fopen("settings.ini", "w");
    if (save_file != NULL) {
        //print directories 
        fprintf(save_file, "[Directories]\n");
        fprintf(save_file, "Path360=%s\n", config.game_repo_360);
        fprintf(save_file, "PathOG=%s\n", config.game_repo_og);
        fprintf(save_file, "GAMESLIST=%s\n", config.games_list_path);
        fprintf(save_file, "PathDLC=%s\n", config.dlc_path);
        fprintf(save_file,"\n\n");
        //print Preferences
        fprintf(save_file, "[Preferences]\n");
        fprintf(save_file, "AlwaysDownloadDlC=%d\n",dlc);


        // 3. Flush to disk and close
        fclose(save_file);
        printf("\n[SUCCESS] Configuration saved to settings.ini\n");
    } else {
        printf("\n[ERROR] Failed to write settings.ini to disk.\n");
    }

    return config;
}
