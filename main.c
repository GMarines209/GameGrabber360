#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <windows.h>
#include <ctype.h>
#include <sys/stat.h>

#define MIN(x, y, z) ((x) < (y) ? ((x) < (z) ? (x) : (z)) : ((y) < (z) ? (y) : (z)))

int transferred,skipped,total;
long long total_size;
FILE* skipped_games;

struct game_match
{
    char dir_name[256];
    int lev_distance;
    float confidence;
}game_match;


int getType(const char* fileName)
{
    struct __stat64 path;

    if (_stat64(fileName, &path) != 0) {
        printf("\nStat failed for: %s\n", fileName); 
        perror("\nError was\n");                    
        return 0; 
    }

    if(S_ISDIR(path.st_mode) != 0){ //dir return 1
        //printf("\nIM A PATH\n");
        return 1;
    }
    else if(S_ISREG(path.st_mode) != 0){ //file returns 2
        //printf("IM A FILE");
        return 2;
    }
    else{
        //printf("\nIM NOTHING\n");
        return 0; // else returns 0
    }
}

long long get_dir_size(char* path){

    long long size = 0;
  
    struct __stat64 buffer;
    struct dirent *entry;

    //  base case - if path is a file not dir then return the size 
    if((getType(path) == 2)){
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
        char fullpath[256];
        sprintf(fullpath,"%s\\%s",path,name);
        size += get_dir_size(fullpath);
    }
    
    closedir(game_dir);
    return size;
}


void moveFolder(char* game_name,char* bestMatch,char* dest_path,char dry_run){
    
    char* flags = "/E /NFL /NDL /NJH /NJS /nc /ns /np";

    if(dry_run == 'y'){
        flags = "/E /NFL /NDL /NJH /NJS /nc /ns /np /L";     //the /L flag is used for dry running
        char full_path[256];
        sprintf(full_path, "E:\\%s", bestMatch);
        total_size += get_dir_size(full_path);
    }

    printf("Transfering: %s...",game_name);
    fflush(stdout);

    char command[512];
    sprintf(command, "robocopy \"E:\\%s\" \"%s\\%s\" %s", bestMatch, dest_path, bestMatch, flags);

    int exitCode = system(command);

    if (exitCode >= 1 && exitCode < 8) {
        printf(" Complete!\n");
        transferred++;
        total++;
    } else {
        printf(" Error (Code: %d).\n", exitCode);
        skipped++;
        total++;
    }
        
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

void toLowerString(char* str){
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

int search_repo(char* game_name, char* dest_path,char dry_run){
    
    //init game repo directory
    DIR *game_repo;
    struct dirent *entry;

    //make case insensitive and remove new line
    toLowerString(game_name);
    game_name[strcspn(game_name, "\n")] = 0;

    game_repo = opendir("E:\\");
    if (game_repo == NULL) {
        printf("Could not open E drive.\n");
        return 1;
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
        moveFolder(game_name, game.dir_name,dest_path,dry_run);
    }else{
        printf("Skipped: %s\n",game_name);
        skipped_games = fopen("skipped.txt","a");
        fprintf(skipped_games,"%s\n",game_name);
        fclose(skipped_games);
        skipped++;
        total++;
    }
    

    closedir(game_repo);
    return 0;
}

int main() {

    //1. Dry run option
    printf("Want to run a dry run?\nY/N: ");
    char dry_run = getchar();
    while(getchar() != '\n');
    dry_run = tolower(dry_run);

    //2. Get destination location
    printf("What is the Destination Filepath? ");
    char dest_Path[256];
    fgets(dest_Path,256,stdin);
    dest_Path[strcspn(dest_Path, "\n")] = 0;
    
    //hard code destination bc its always the same
    FILE *client_list_file = fopen("C:\\Users\\Gabriel\\Downloads\\games.txt", "r");

    
    if (client_list_file == NULL) {
        printf("File could not be opened.\n"); 
        return 1; 
    } else {

        char game_buffer[256]; 
        while (fgets(game_buffer, 256, client_list_file) != NULL) {
            search_repo(game_buffer,dest_Path,dry_run);
        }

        printf("\n=== Summary ===\n");
        printf("Transferred: %d\n",transferred);
        printf("Skipped: %d\n",skipped);
        printf("Total: %d\n",total);

        double total_GB = total_size / 1073741824.0;
        printf("Total Size is: %.2f GB",total_GB);
        
        fclose(client_list_file); 
    }

    return 0;
}



