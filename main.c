#include <stdio.h>
#include <string.h> 

int main() {

    // 1. Take in requested games list
    printf("What is the filepath to the client text file? ");

    char client_list_path[256];
    fgets(client_list_path, 256, stdin);

    client_list_path[strcspn(client_list_path, "\n")] = 0; 

    printf("Where are the games stored?");
    char games_path[256];
    fgets(games_path, 256, stdin);
    games_path[strcspn(games_path, "\n")] = 0; 

    FILE *client_list_file = fopen(client_list_path, "r");

    if (client_list_file == NULL) {
        printf("File could not be opened.\n"); 
        return 1; 
    } else {

        //process line by line
        char line_buffer[256]; 

        while (fgets(line_buffer, 256, client_list_file) != NULL) {
            
        }
        
        fclose(client_list_file); 
    }

    return 0;
}