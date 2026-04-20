#ifndef CONFIG_H  
#define CONFIG_H

#include "structs.h"
#include <stdbool.h> 


appConfig load_settings();
appConfig first_time_setup();
void prompt_for_path(const char* prompt_text, char* destination_buffer, bool is_file);


#endif