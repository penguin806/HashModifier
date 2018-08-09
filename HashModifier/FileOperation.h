#pragma once
#define MAX_DRAG_FILE 10

#ifdef UNICODE
#define FOPEN _wfopen
#else
#define FOPEN fopen
#endif

struct FilePathList
{
	LPTSTR szFilePath;
	UINT uCurrentFileNum;
};
typedef struct FilePathList FILEPATHLIST;


BOOL SetValue(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], UINT *uFileNum, UINT *uSuccessNum);
UINT HashMod(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], UINT uFileNum);