#pragma once

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
VOID SplitOneMultiLineStringIntoFilePathList(_In_ LPTSTR szMultilineString, _In_ UINT uStringLength, _Inout_ FILEPATHLIST *PathList);
VOID ModifyHashOfEachFileInList(_In_ FILEPATHLIST *PathList, _Out_ UINT *uSuccuessNum, _Out_ UINT *uFailedNum);