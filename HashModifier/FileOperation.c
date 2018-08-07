#include <stdio.h>
#include <Windows.h>
#include "FileOperation.h"

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