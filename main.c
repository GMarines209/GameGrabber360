#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define MIN(x, y, z) ((x) < (y) ? ((x) < (z) ? (x) : (z)) : ((y) < (z) ? (y) : (z)))

int moveFolder(){

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

//  C:\Users\Gabriel\Downloads\games.txt


//  C:\Users\Gabriel\OneDrive\Desktop\Test

void toLowerString(char* str){
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

int search_repo(char* game_name, char* dest_path){
    
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

    int bestDist = 999;
    char bestmatch[256];
    bestmatch[0] = '\0';
    
    // loop through game repo and find file name matches
    while ((entry = readdir(game_repo)) != NULL)
    {

        toLowerString(entry->d_name);
        int dist = Lev_Distance(entry->d_name,game_name);
        int len = strlen(game_name);
        
        int threshold = len / 4;
        if (threshold < 3) threshold = 3;

        
        //loop and save best distance
        if (dist <= threshold) {
            if(dist < bestDist){
                bestDist = dist;
                strcpy(bestmatch,entry->d_name);
            }
        }
    }

    if(bestDist < 999){
        printf("The best distance for %s was %s at %d\n",game_name,bestmatch,bestDist);

        char command[512];
        sprintf(command, "robocopy \"E:\\%s\" \"%s\\%s\" /E", bestmatch, dest_path, bestmatch);
        printf("Executing: %s\n", command);
        system(command);
    }
    else printf("%s had no valid match\n",game_name);
    
}

int main() {

    // // 1. Take in requested games list
    // printf("What is the filepath to the client text file? ");
    // char client_list_path[256];
    // fgets(client_list_path, 256, stdin);
    // client_list_path[strcspn(client_list_path, "\n")] = 0; 

    //1. Get destination location
    printf("What is the Destination Filepath? ");
    char dest_Path[256];
    fgets(dest_Path,256,stdin);
    dest_Path[strcspn(dest_Path, "\n")] = 0;
    
    FILE *client_list_file = fopen("C:\\Users\\Gabriel\\Downloads\\games.txt", "r");

    if (client_list_file == NULL) {
        printf("File could not be opened.\n"); 
        return 1; 
    } else {

        //process line by line
        char line_buffer[256]; 
        line_buffer[strcspn(line_buffer, "\n")] = 0;


        while (fgets(line_buffer, 256, client_list_file) != NULL) {
            search_repo(line_buffer,dest_Path);
        }
        
        fclose(client_list_file); 
    }

    return 0;
}



