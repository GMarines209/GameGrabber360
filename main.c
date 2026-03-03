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

void moveFolder(char* bestMatch, struct menu_selection my_selection, appContext *context_ptr){
    
    int dry_run = my_selection.run_mode;
    
    char final_flag[512];
    strcpy(final_flag, "/E /NFL /NDL /NJH /NJS /nc /ns /np"); 

    char source_path[512];
    if(my_selection.game_source == 2){
        snprintf(source_path,sizeof(source_path), "E:\\1-Original Xbox games\\%s", bestMatch);
    } else {
        snprintf(source_path,sizeof(source_path), "E:\\%s", bestMatch);
    }

    if(dry_run == 1){
        strcat(final_flag, " /L"); //L flag is for listing only / dry run
        
        context_ptr->total_size += get_dir_size(source_path);
    }

    char dest_path[256];
    strcpy(dest_path, my_selection.dest_Path);

    char command[1024];
    snprintf(command, sizeof(command), "robocopy \"%s\" \"%s\\%s\" %s > nul", source_path, dest_path, bestMatch, final_flag);
    
    int exitCode = system(command);

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


int search_repo(char* game_name, struct menu_selection my_selection, appContext *context_ptr){
    
    //init game repo directory
    DIR *game_repo;
    struct dirent *entry;

    int dry_run = my_selection.run_mode;

    char dest_path[256];
    strcpy(dest_path,my_selection.dest_Path);

    //make case insensitive and remove new line
    game_name[strcspn(game_name, "\n")] = 0;

    printf("Transferring: %s... ", game_name); 
    fflush(stdout);

    toLowerString(game_name);
    
    if(my_selection.game_source == 1){

        game_repo = opendir("E:\\");
        if (game_repo == NULL) {
            printf("Could not open E:\\ drive.\n");
            return 1;
        }
    }
    else{
        game_repo = opendir("E:\\1-Original Xbox games");
        if (game_repo == NULL) {
            printf("Could not open E:\\original Xbox games drive.\n");
            return 1;
        }
    }
    

    //Init struct of game and set lev distance to max for comparison
    struct game_match game;
    game.lev_distance = 999;
    
    // loop through game repo and find file name matches
    while ((entry = readdir(game_repo)) != NULL)
    {
        char lower_Dir_Name[256];
        strncpy(lower_Dir_Name,entry->d_name,255);
        lower_Dir_Name[255] = '\0';
        toLowerString(lower_Dir_Name);

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
        moveFolder(game.dir_name,my_selection,context_ptr);
    }else{
        printf("\xe2\x9d\x8c Skipped (no match)\n"); // print cool chars
        fprintf(context_ptr->skipped_games_log,"%s\n",game_name);
        context_ptr->skipped++;
        context_ptr->total_count++;
    }
    

    closedir(game_repo);
    return 0;
}

selection menu(){

    selection my_selection;
    int choice;
    
    printf("==== Game Grabber 360 ====\n");
    printf("Select game source:\n");
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
    char* selected_path = PickFolder(NULL);
    switch (choice)
    {
    case 1:
        printf("\nWhat is the Destination Filepath? ");
        if (selected_path != NULL) {
            // Copy it into struct safely
            strncpy(my_selection.dest_Path, selected_path, 255);
            my_selection.dest_Path[255] = '\0'; 
            
            free(selected_path); 
        } else {
            printf("No folder selected. Defaulting to current directory.\n");
        }

        my_selection.run_mode = 1; //dry run
        break;

    case 2:
        printf("\nWhat is the Destination Filepath? ");
        if (selected_path != NULL) {
            strncpy(my_selection.dest_Path, selected_path, 255);
            my_selection.dest_Path[255] = '\0';
            
            free(selected_path); 
        } else {
            printf("No folder selected. Defaulting to current directory.\n");
        }

        my_selection.run_mode = 2; // normal run
        break;
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

    appContext context;

    context.transfered = 0;
    context.skipped = 0;
    context.total_count = 0;
    context.total_size = 0;
    context.skipped_games_log = NULL;

    selection my_selection = menu();
    if (my_selection.game_source == -1 || my_selection.run_mode == -1) {
        printf("Exiting...\n");
        return 0; 
    }

    //open games list 
    FILE *client_list_file = fopen("C:\\Users\\Gabriel\\OneDrive\\Desktop\\Xbox stuff\\Client stuff\\games.txt", "r");


    // Generate unique skipped filename
    char filename[100];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(filename, sizeof(filename), "Skipped_%Y-%m-%d_%H%M%S.txt", t);

    
    if (client_list_file == NULL) {
        printf("File could not be opened.\n"); 
        return 1; 
    } else {
        clock_t start = clock();
        char game_buffer[256]; 
        context.skipped_games_log = fopen(filename,"a");
        while (fgets(game_buffer, 256, client_list_file) != NULL) {
            search_repo(game_buffer,my_selection,&context);
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
        clock_t end = clock();

        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken: %.2f seconds\n", time_spent);
    }

    printf("\n\n");
    fclose(context.skipped_games_log);
    system("pause");
    return 0;
}



