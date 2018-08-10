#pragma once
#define MAX_DRAG_FILE 10

#ifdef UNICODE
#define FOPEN _wfopen
#else
#define FOPEN fopen
#endif

struct FilePathInfoNode
{
	LPTSTR szFilePathString;
	struct FilePathInfoNode *Next;
};
typedef struct FilePathInfoNode FILEPATHINFONODE;

struct FilePathList
{
	FILEPATHINFONODE *Head;
	FILEPATHINFONODE *Tail;
};
typedef struct FilePathList FILEPATHLIST;


BOOL AppendPathToFilePathList(_In_ LPTSTR szFilePathStr, _Inout_ FILEPATHLIST *PathList);
VOID DestroyFilePathList(_Inout_ FILEPATHLIST *PathList);
UINT CalcBytesRequiredForMergeFilePathList(_In_ FILEPATHLIST *PathList);
VOID MergeFilePathListIntoOneMultiLineString(_Inout_ LPTSTR szMultilineString, _In_ UINT uBufferSizeInBytes, _In_ FILEPATHLIST *PathList);
BOOL SetValue(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], UINT *uFileNum, UINT *uSuccessNum);
UINT HashMod(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], UINT uFileNum);