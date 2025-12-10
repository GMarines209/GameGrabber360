#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <math.h>

int Lev_Distance(char* str1, char* str2){
    
    int len1 = strlen(str1);
    int len2 = strlen(str2);


}

//  C:\Users\Gabriel\Downloads\games.txt

void toLowerString(char* str){
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

int search_repo(char* game_name){
    
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
    
    // loop through game repo and find file name matches
    while ((entry = readdir(game_repo)) != NULL)
    {
        toLowerString(entry->d_name);
        if (strstr(entry->d_name, game_name) != NULL)
        {
            printf("Found match: %s\n", entry->d_name);
        }
        
    }
    


}

int main() {

    // 1. Take in requested games list
    printf("What is the filepath to the client text file? "); 

    char client_list_path[256];
    fgets(client_list_path, 256, stdin);
    client_list_path[strcspn(client_list_path, "\n")] = 0; 
    
    FILE *client_list_file = fopen(client_list_path, "r");

    if (client_list_file == NULL) {
        printf("File could not be opened.\n"); 
        return 1; 
    } else {

        //process line by line
        char line_buffer[256]; 
        line_buffer[strcspn(line_buffer, "\n")] = 0;


        while (fgets(line_buffer, 256, client_list_file) != NULL) {
            search_repo(line_buffer);
        }
        
        fclose(client_list_file); 
    }

    return 0;
}



