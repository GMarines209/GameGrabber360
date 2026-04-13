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

void move_folder(char* bestMatch, struct menu_selection my_selection, appContext *context_ptr, appConfig *config_ptr){
        
    char final_flag[MAX_BUFFER];
    strcpy(final_flag, "/E /NDL /NJH /NJS /nc /BYTES /np"); 


    //real run
    char source_path[MAX_PATH_SIZE];
    if(my_selection.game_source == 2){
        snprintf(source_path,sizeof(source_path), "%s\\%s", config_ptr->game_repo_og,bestMatch);
    } else {
        snprintf(source_path,sizeof(source_path), "%s\\%s", config_ptr->game_repo_360,bestMatch);     //HARDCODED
    }

    long long total_bytes = get_dir_size(source_path);

    //dry run
    if(my_selection.run_mode == 1){
        strcat(final_flag, " /L"); //L flag is for listing only / dry run
        
        context_ptr->total_size += total_bytes;
    }

    char dest_path[MAX_PATH_SIZE];
    strcpy(dest_path, my_selection.dest_Path);

    char command[2048];
    snprintf(command, sizeof(command), "robocopy \"%s\" \"%s\\%s\" %s", source_path, dest_path, bestMatch, final_flag);

    long long bytes_transferred = 0;

    //swithced to _popen instead of system() for real time command line updating for file progress display
    FILE *pipe = _popen(command, "r");
    char buffer[MAX_BUFFER];
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL){
        
        long long file_bytes = 0;
        
        if (sscanf(buffer, "%lld", &file_bytes) == 1) {
            
            bytes_transferred += file_bytes;
            
            float progress = 0.0f;
            
            if (total_bytes > 0) {
                progress = ((float)bytes_transferred / (float)total_bytes) * 100.0f;
            }
            
            printf("\rTransferring: %s... %.1f%%", bestMatch, progress);
            fflush(stdout); 
        }
    }

    int exitCode = _pclose(pipe);

    if (exitCode >= 0 && exitCode < 8) {
        printf(" Complete \xe2\x9c\x94\n");
        context_ptr->transfered++;
        context_ptr->total_count++;
    } else {
        printf(" Error (Code: %d).\n", exitCode);
        context_ptr->skipped++;
        context_ptr->total_count++;
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

        game_repo = opendir(config_ptr->game_repo_360); //HARDCODED
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
    context.transfered = 0;
    context.skipped = 0;
    context.total_count = 0;
    context.total_size = 0;
    context.skipped_games_log = NULL;
    context.go_again = 1;

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
            printf("Transferred: %d\n",context.transfered);
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
        context.transfered = 0;
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



