#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0900

#define COBJMACROS
#include <windows.h>
#include <shobjidl.h>
#include <stdio.h>
#include "file_browser.h"

// release COM objects
static void SafeRelease(IUnknown **ppT) {
    if (*ppT) {
        IUnknown_Release(*ppT);
        *ppT = NULL;
    }
}

char* PickFolder(HWND owner) {
    IFileDialog *pFileDialog = NULL;
    IShellItem *pItem = NULL;
    char *pszPathMultiByte = NULL; // holds the final char* path

    // init COM
    HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Create the FileOpenDialog
    HRESULT hr = CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                                  &IID_IFileDialog, (void**)(&pFileDialog));

    if (SUCCEEDED(hr)) {
        DWORD dwOptions;
        if (SUCCEEDED(IFileDialog_GetOptions(pFileDialog, &dwOptions))) {
            IFileDialog_SetOptions(pFileDialog, dwOptions | FOS_PICKFOLDERS);
        }

        if (SUCCEEDED(IFileDialog_Show(pFileDialog, owner))) {
            if (SUCCEEDED(IFileDialog_GetResult(pFileDialog, &pItem))) {
                PWSTR tempPathW;
                
                // Get the path as a Wide String (WCHAR)
                if (SUCCEEDED(IShellItem_GetDisplayName(pItem, SIGDN_FILESYSPATH, &tempPathW))) {
                    
                    // --- CONVERSION LOGIC (WCHAR -> char)
                    // 1. Calculate required size
                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, tempPathW, -1, NULL, 0, NULL, NULL);
                    
                    // 2. Allocate memory for standard char string
                    pszPathMultiByte = (char*)malloc(size_needed);
                    
                    // 3. Convert
                    if (pszPathMultiByte) {
                        WideCharToMultiByte(CP_UTF8, 0, tempPathW, -1, pszPathMultiByte, size_needed, NULL, NULL);
                    }

                    CoTaskMemFree(tempPathW); // Free the Windows string
                }
                SafeRelease((IUnknown**)&pItem);
            }
        }
        SafeRelease((IUnknown**)&pFileDialog);
    }

    if (SUCCEEDED(hrInit)) {
        CoUninitialize();
    }

    return pszPathMultiByte;
}
