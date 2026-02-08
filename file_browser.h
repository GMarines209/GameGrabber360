#ifndef FILE_BROWSER_H  
#define FILE_BROWSER_H

#include <windows.h>

// open folder picker dialog and return path as string
// must free() the returned string
char* PickFolder(HWND owner);

#endif