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
	*szMultilineString = 0;

	while (pNode)
	{
		StringCbCat(szMultilineString, uBufferSizeInBytes, pNode->szFilePathString);
		StringCbCat(szMultilineString, uBufferSizeInBytes, TEXT("\r\n"));
		pNode = pNode->Next;
	}
}

VOID SplitOneMultiLineStringIntoFilePathList(_In_ LPTSTR szMultilineString, _In_ UINT uStringLength, _Inout_ FILEPATHLIST *PathList)
{
	if (NULL == szMultilineString || NULL == PathList || 0 == uStringLength)
	{
		return;
	}

	LPTSTR pStart, pCurrent, pEnd;
	pStart = pCurrent = szMultilineString, pEnd = szMultilineString + uStringLength;
	while (*pCurrent && pCurrent < pEnd)
	{
		if (pCurrent == pStart && (TEXT('\r') == *pCurrent || TEXT('\n') == *pCurrent || TEXT(' ') == *pCurrent))
		{
			pStart = ++pCurrent;
		}
		else if (TEXT('\r') == *pCurrent && TEXT('\n') == *(pCurrent + 1))
		{
			*pCurrent = 0;
			pCurrent += 2;
			AppendPathToFilePathList(pStart, PathList);
			pStart = pCurrent;
		}
		else
		{
			pCurrent++;
		}
	}

	if (0 == *pCurrent && 0 != *pStart && pStart <= pEnd)
	{
		AppendPathToFilePathList(pStart, PathList);
	}
}

VOID ModifyHashOfEachFileInList(_In_ FILEPATHLIST *PathList, _Out_ UINT *uSuccuessNum, _Out_ UINT *uFailedNum)
{
	if (NULL == PathList || NULL == uSuccuessNum || 0 == uFailedNum)
	{
		return;
	}

	UINT uSuccess = 0, uFailed = 0;
	HANDLE hFileToMod;
	FILEPATHINFONODE *pNode = PathList->Head;

	while(pNode)
	{
		hFileToMod = CreateFile(pNode->szFilePathString, FILE_APPEND_DATA,
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFileToMod == INVALID_HANDLE_VALUE)
		{
			uFailed++;
		}
		else
		{
			DWORD dResult;
			dResult = SetFilePointer(hFileToMod, 0, NULL, FILE_END);
			if (INVALID_SET_FILE_POINTER == dResult)
			{
				uFailed++;
				CloseHandle(hFileToMod);
			}
			else
			{
				CONST BYTE ZeroByte = 0;
				DWORD ActualWritten;
				BOOL bResult;
				bResult = WriteFile(hFileToMod, &ZeroByte, 1, &ActualWritten, NULL);
				if (FALSE == bResult || ActualWritten != 1)
				{
					uFailed++;
					CloseHandle(hFileToMod);
				}
				else
				{
					uSuccess++;
					CloseHandle(hFileToMod);
				}
			}
		}

		pNode = pNode->Next;
	}

	*uSuccuessNum = uSuccess;
	*uFailedNum = uFailed;
	return;
}