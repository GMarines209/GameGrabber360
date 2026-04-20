#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <shobjidl.h> 


#include "file_browser.h"
#include "utils.h"
#include "structs.h"
#include "config.h"

int transfer_pipe(char* command, appContext *context_ptr, long long total_bytes, char* display_text) {
    FILE *pipe = _popen(command, "r");
    char buffer[MAX_BUFFER];
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        long long file_bytes = 0;
        
        if (sscanf(buffer, "%lld", &file_bytes) == 1) {
            context_ptr->bytes_transferred += file_bytes;
            float progress = 0.0f;
            
            if (total_bytes > 0) {
                progress = ((float)context_ptr->bytes_transferred / (float)total_bytes) * 100.0f;
            }
            
            printf("\r%s ... %.1f%%", display_text, progress);
            fflush(stdout);
        }
    }
    
    int exitCode = _pclose(pipe);
    return exitCode;
}

void move_folder(char* bestMatch, struct menu_selection my_selection, appContext *context_ptr, appConfig *config_ptr){
        
    char final_flag[MAX_BUFFER];
    strcpy(final_flag, "/E /NDL /NJH /NJS /nc /BYTES /np"); 

    

    //full run
    char source_path[MAX_PATH_SIZE * 2];
    if(my_selection.game_source == 2){ //if its a orignal xbox game
        snprintf(source_path,sizeof(source_path), "%s\\%s", config_ptr->game_repo_og,bestMatch);
    } else {//if its a xbox360 game
        snprintf(source_path,sizeof(source_path), "%s\\%s", config_ptr->game_repo_360,bestMatch);    
    }
    long long total_bytes;
    total_bytes = get_dir_size(source_path);

    //dry run
    if(my_selection.run_mode == 1){
        strcat(final_flag, " /L"); //L flag is for listing only / dry run
        context_ptr->total_size += total_bytes;
    }

    char dest_path[MAX_PATH_SIZE];
    strcpy(dest_path, my_selection.dest_Path);

    char command[MAX_COMMAND* 2];
    snprintf(command, sizeof(command), "robocopy \"%s\" \"%s\\%s\" %s", source_path, dest_path, bestMatch, final_flag);

    char display_base[MAX_BUFFER];
    snprintf(display_base, sizeof(display_base), "Transferring: %s", bestMatch);

    int exitCode = transfer_pipe(command, context_ptr, total_bytes, display_base);

    if (exitCode >= 0 && exitCode < 8) {
        printf(" Complete \xe2\x9c\x94\n");
        context_ptr->transferred++;
        context_ptr->total_count++;
    } else {
        printf(" Error (Code: %d).\n", exitCode);
        context_ptr->skipped++;
        context_ptr->total_count++;
    }

    //if we need to load dlc
    if(config_ptr->load_dlc == 1){       
        char final_dlc_path[MAX_PATH_SIZE * 2];
        snprintf(final_dlc_path,sizeof(final_dlc_path), "%s\\%s", config_ptr->dlc_path, bestMatch);

        DIR* dlc_dir = opendir(final_dlc_path);
        if(dlc_dir != NULL){
            //close if it exits bc its not needed anymore
            closedir(dlc_dir);

            context_ptr->bytes_transferred = 0;
            long long total_dlc_bytes = get_dir_size(final_dlc_path);

            char dlc_command[MAX_COMMAND * 2];
            snprintf(dlc_command, sizeof(dlc_command), "robocopy \"%s\" \"%s\" %s", final_dlc_path, dest_path, final_flag);

            //costruct display text for the dlcs
            char display_dlc[MAX_BUFFER];
            snprintf(display_dlc, sizeof(display_dlc), "\t└─ transfering %s DLC:", bestMatch);

            exitCode = transfer_pipe(dlc_command,context_ptr,total_dlc_bytes,display_dlc);

            if (exitCode >= 0 && exitCode < 8) {
                printf(" Complete \xe2\x9c\x94\n");
            } else {
                printf(" Error (Code: %d).\n", exitCode);
            }
        }
    }
    
}

int search_repo(char* game_name, struct menu_selection my_selection, appContext *context_ptr,appConfig *config_ptr){
    
    //init game repo directory
    DIR *game_repo;
    struct dirent *entry;

    char dest_path[512];
    strcpy(dest_path,my_selection.dest_Path);

    //make case insensitive and remove new line
    game_name[strcspn(game_name, "\n")] = 0;

    printf("Transferring: %s... ", game_name); 
    fflush(stdout);

    to_lower_string(game_name);
    
    if(my_selection.game_source == 1){

        game_repo = opendir(config_ptr->game_repo_360); 
        if (game_repo == NULL) {
            printf("Could not open %s\n",config_ptr->game_repo_360);
            return 1;
        }
    }
    else{
        game_repo = opendir(config_ptr->game_repo_og); 
        if (game_repo == NULL) {
            printf("Could not open %s\n",config_ptr->game_repo_og);
            return 1;
        }
    }
    

    //Init struct of game and set lev distance to max for comparison
    struct game_match game;
    game.lev_distance = 999;
    
    // loop through game repo and find file name matches
    while ((entry = readdir(game_repo)) != NULL)
    {
        char lower_Dir_Name[MAX_PATH_SIZE];
        strncpy(lower_Dir_Name,entry->d_name,255);
        lower_Dir_Name[255] = '\0';
        to_lower_string(lower_Dir_Name);

        int dist = Lev_Distance(lower_Dir_Name,game_name);
        int len = strlen(game_name);

        int threshold = len / 4;
        if(threshold < 2){
            threshold = 2;
        }

        //loop and save best distance
        if (dist <= threshold) {

            if(dist < game.lev_distance){
                
                game.lev_distance = dist;
                strcpy(game.dir_name,entry->d_name);
            }
        }
    }   

    if(game.lev_distance < 999){
        move_folder(game.dir_name,my_selection,context_ptr,config_ptr);
        context_ptr->bytes_transferred = 0;// reset count after each game/dlc call
    }else{
        printf("\xe2\x9d\x8c Skipped (no match)\n"); // print cool chars

        //if the skipped games log is empty
        if(context_ptr->skipped_games_log == NULL){
            // Generate unique skipped filename
            char filename[MAX_BUFFER];
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(filename, sizeof(filename), "Skipped_%Y-%m-%d_%H%M%S.txt", t);

            //open the skipped games log file
            context_ptr->skipped_games_log = fopen(filename,"a");
        }

        if(context_ptr->skipped_games_log != NULL){
            fprintf(context_ptr->skipped_games_log,"%s\n",game_name);
            fflush(context_ptr->skipped_games_log);
        }

        context_ptr->skipped++;
        context_ptr->total_count++;
    }
    

    closedir(game_repo);
    return 0;
}

selection menu(){

    selection my_selection;
    int choice;
    
    printf("\nSelect game source:\n");
    printf("1. Xbox 360 Games\n");
    printf("2. Original Xbox Games\n");
    printf("3. Exit\n");

    scanf("%d",&choice);
    while ((getchar()) != '\n');

    
    while(choice != 1 && choice != 2 && choice != 3){
        printf("Invalid selection. Try again:\t");
        scanf("%d",&choice);
        while ((getchar()) != '\n');
    }
    my_selection.game_source = choice;
    if(choice == 3){
        my_selection.game_source = -1;
        my_selection.run_mode = -1;
        return my_selection;
    }

   
    printf("\nSelect Mode\n");
    printf("1. Dry Run\n");
    printf("2. Full Transfer\n");
    printf("3. Exit\n");

    scanf("%d",&choice);
    while ((getchar()) != '\n');


    while(choice != 1 && choice != 2 && choice != 3){
        printf("\nInvalid selection. Try again\n");
        scanf("%d",&choice);
        while ((getchar()) != '\n');
    }
    
    my_selection.run_mode = choice;
    
    printf("\nWhat is the Destination Filepath? ");
    char* selected_path = PickFolder(NULL);

    switch (choice)
    {
    case 1:
        
        if(selected_path == NULL){
            printf("\nSelection cancelled. Aborting program...\n");
            my_selection.game_source = -1;
            my_selection.run_mode = -1;
            return my_selection;
        }
        else{
            // Copy it into struct safely
            strncpy(my_selection.dest_Path, selected_path, MAX_PATH_SIZE -1);
            my_selection.dest_Path[MAX_PATH_SIZE -1] = '\0'; 
            
            free(selected_path); 
            my_selection.run_mode = 1;
            break;
        }

    case 2:
        
        if(selected_path == NULL){
            printf("\nSelection cancelled. Aborting program...\n");
            my_selection.game_source = -1;
            my_selection.run_mode = -1;
            return my_selection;
        }
        else{
            // Copy it into struct safely
            strncpy(my_selection.dest_Path, selected_path, MAX_PATH_SIZE -1);
            my_selection.dest_Path[MAX_PATH_SIZE -1] = '\0'; 
            
            free(selected_path); 
            my_selection.run_mode = 2; //Full run
            break;
        }
    case 3:
        my_selection.game_source = -1;
        my_selection.run_mode = -1;
        return my_selection;
    default:
        break;
    }
    
    
    return my_selection;

}

int main() {

    SetConsoleOutputCP(65001);

    //initilize context struct
    appContext context;

    //initilize context 
    context.transferred = 0;
    context.skipped = 0;
    context.total_count = 0;
    context.total_size = 0;
    context.skipped_games_log = NULL;
    context.go_again = 1;
    context.bytes_transferred = 0;

    print_splash_screen();
    appConfig my_config = load_settings();
    
        
    while(context.go_again == 1){

        selection my_selection = menu();
        if (my_selection.game_source == -1 || my_selection.run_mode == -1) {
            break; 
        }

        printf("\n");
        //open games list 
        FILE *client_list_file = fopen(my_config.games_list_path, "r"); 
        
        if (client_list_file == NULL) {
            printf("File could not be opened.\n"); 
            return 1; 
        } else {
            time_t start_time = time(NULL);
            char game_buffer[MAX_BUFFER]; 
            while (fgets(game_buffer, MAX_BUFFER, client_list_file) != NULL) {
                search_repo(game_buffer,my_selection,&context,&my_config);
            }

            printf("\n=== Summary ===\n");
            printf("Transferred: %d\n",context.transferred);
            printf("Skipped: %d\n",context.skipped);
            printf("Total: %d\n",context.total_count);

            if(my_selection.run_mode == 1){
                double total_GB = context.total_size / 1073741824.0;
                printf("Total Size is: %.2f GB\n",total_GB);
            }
            
            fclose(client_list_file); 
            time_t end_time = time(NULL);
            double time_spent = difftime(end_time,start_time);

            printf("Time taken: %.2f seconds\n", time_spent);

            printf("\nWould you like to go again? ([1]Yes/[0]No) ");
            scanf("%d",&context.go_again);
            while(context.go_again != 0 && context.go_again != 1 ){
                printf("\nInvalid selection. Try again\n");
                scanf("%d",&context.go_again);
                while ((getchar()) != '\n');
            }
            

        }

        context.total_size = 0;
        context.skipped = 0;
        context.total_count = 0;
        context.transferred = 0;
        printf("\n\n");
    }

    printf("\n\n");
    if(context.skipped_games_log != NULL){
        fclose(context.skipped_games_log);
    }
    
    printf("Press Enter to exit...");
    getchar();
    return 0;
}



