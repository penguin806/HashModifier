#include <stdio.h>
#include <Windows.h>
#include <strsafe.h>
#include "FileOperation.h"

BOOL AppendPathToFilePathList(_In_ LPTSTR szFilePathStr, _Inout_ FILEPATHLIST *PathList)
{
	if (NULL == szFilePathStr || NULL == PathList)
	{
		return FALSE;
	}
	UINT uLength;
	LRESULT lResult;
	lResult = StringCchLength(szFilePathStr, MAX_PATH, &uLength);
	if (FAILED(lResult))
	{
		return FALSE;
	}

	LPTSTR szFilePathToAppend = LocalAlloc(LMEM_FIXED, (uLength + 1) * sizeof(TCHAR));
	if (NULL == szFilePathToAppend)
	{
		return FALSE;
	}

	lResult = StringCchCopy(szFilePathToAppend, (uLength + 1) * sizeof(TCHAR), szFilePathStr);
	if (FAILED(lResult))
	{
		LocalFree(szFilePathToAppend);
		return FALSE;
	}
	
	FILEPATHINFONODE *NewPathNode = LocalAlloc(LMEM_FIXED, sizeof(FILEPATHINFONODE));
	if (NULL == NewPathNode)
	{
		LocalFree(szFilePathToAppend);
		return FALSE;
	}
	else
	{
		NewPathNode->szFilePathString = szFilePathToAppend;
		NewPathNode->Next = NULL;
	}

	if (NULL == PathList->Head)
	{
		PathList->Head = PathList->Tail = NewPathNode;
	}
	else
	{
		PathList->Tail->Next = NewPathNode;
		PathList->Tail = NewPathNode;
	}
	return TRUE;
}

VOID DestroyFilePathList(_Inout_ FILEPATHLIST *PathList)
{
	if (NULL == PathList)
	{
		return;
	}
	FILEPATHINFONODE *pNode = PathList->Head;
	while (pNode)
	{
		LocalFree(pNode->szFilePathString);
		pNode = pNode->Next;
	}

	ZeroMemory(PathList, sizeof(FILEPATHLIST));
}

UINT CalcBytesRequiredForMergeFilePathList(_In_ FILEPATHLIST *PathList)
{
	if (NULL == PathList)
	{
		return 0;
	}
	FILEPATHINFONODE *pNode = PathList->Head;
	UINT BytesRequired = 0;

	while (pNode)
	{
		BytesRequired += LocalSize(pNode->szFilePathString);
		BytesRequired += sizeof(TCHAR) * 2;	//Character '\r' and '\n'
		pNode = pNode->Next;
	}

	return BytesRequired;
}

VOID MergeFilePathListIntoOneMultiLineString(_Inout_ LPTSTR szMultilineString, _In_ UINT uBufferSizeInBytes, _In_ FILEPATHLIST *PathList)
{
	if (NULL == szMultilineString || NULL == PathList || 0 == uBufferSizeInBytes)
	{
		return;
	}

	FILEPATHINFONODE *pNode = PathList->Head;
	if (NULL == pNode)
	{
		return;
	}

	while (pNode)
	{
		StringCbCat(szMultilineString, uBufferSizeInBytes, pNode->szFilePathString);
		StringCbCat(szMultilineString, uBufferSizeInBytes, TEXT("\r\n"));
		pNode = pNode->Next;
	}
}

BOOL SetValue(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], UINT *uFileNum, UINT *uSuccessNum)
{
	UINT i;
	for (i = 0; i < MAX_DRAG_FILE; i++)
		wcscpy(szFileName[i], TEXT("\0"));
	wcscpy(szNameBuffer, TEXT("\0"));
	*uFileNum = 0;
	*uSuccessNum = 0;
	return TRUE;
}

UINT HashMod(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], UINT uFileNum)
{
	int iResult = 0;
	UINT uCount, uSuccess = 0;
	FILE *fp;
	for (uCount = 0; uCount < uFileNum; uCount++)
	{
		fp = FOPEN(szFileName[uCount], TEXT("ab"));
		if (fp == NULL)
			continue;
		iResult = fseek(fp, 0L, 2);
		if (iResult == -1)
		{
			fclose(fp);
			continue;
		}
		iResult = fwrite("\0", 1, 1, fp);
		if (iResult != 1)
		{
			fclose(fp);
			continue;
		}
		fclose(fp);
		uSuccess++;
	}
	return uSuccess;
}