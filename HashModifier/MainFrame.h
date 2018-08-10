#pragma once
#include "FileOperation.h"
#define ID_EDIT_BOX 0X01
#define ID_BUTTON_OK 0X02
#define ID_BUTTON_ABOUT 0X03
#define ID_BUTTON_WEBSITE 0X04

BOOL RegisterWindowClass(_In_ HINSTANCE hInstance, _In_ LPTSTR szWndClassName);
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
BOOL RetrieveDragFilePath(_In_ HDROP hDrop, _Inout_ FILEPATHLIST *PathList);
VOID DisplayPathListInEditBox(_In_ HWND hEditBox, _Inout_ FILEPATHLIST *PathList);
UINT GetTextBox(TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], TCHAR szFileName[10][MAX_PATH]);