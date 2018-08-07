//Snow 2015-10-23
#include <stdio.h>
#include <Windows.h>
#include "resource.h"

#define MAX_DRAG_FILE 10
#define ID_EDIT_BOX 0X01
#define ID_BUTTON_OK 0X02
#define ID_BUTTON_ABOUT 0X03
#define ID_BUTTON_WEBSITE 0X04

#ifdef UNICODE
#define FOPEN _wfopen
#else
#define FOPEN fopen
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL SetValue(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], UINT *uFileNum, UINT *uSuccessNum);
UINT DragFile(HWND hWnd, HDROP hDrop, TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH]);
BOOL DisplayFormat(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], UINT uFileNum);
UINT GetTextBox(TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], TCHAR szFileName[10][MAX_PATH]);
UINT HashMod(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], UINT uFileNum);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	WNDCLASS WindowClass;
	HWND hWnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Xfen");
	WindowClass.hInstance = hInstance;
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WindowClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR));
	WindowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WindowClass.lpszClassName = szAppName;
	WindowClass.lpszMenuName = NULL;
	if (RegisterClass(&WindowClass) == 0)
	{
		MessageBox(NULL, TEXT("Register Window Class Fail!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return 0;
	}
	hWnd = CreateWindow(szAppName, TEXT("Hash Modifier"), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 345, 250, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
	{
		MessageBox(NULL, TEXT("Create Window Fail!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bStatus = FALSE;
	HINSTANCE hInstance = GetModuleHandle(0);
	static UINT uFileNum, uSuccessNum;
	static TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], szNameBuffer[10 * MAX_PATH], szDisplayBuffer[64];
	static HWND hEditBox, hButtonOk, hButtonAbout, hButtonWebsite;
	switch (uMsg)
	{
	case WM_CREATE:
		hEditBox = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL, 20, 5, 300, 150, hWnd, (HMENU)ID_EDIT_BOX, hInstance, NULL);
		hButtonOk = CreateWindow(TEXT("button"), NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_ICON, 30, 160, 48, 48, hWnd, (HMENU)ID_BUTTON_OK, hInstance, NULL);
		hButtonAbout = CreateWindow(TEXT("button"), NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 145, 160, 48, 48, hWnd, (HMENU)ID_BUTTON_ABOUT, hInstance, NULL);
		hButtonWebsite = CreateWindow(TEXT("button"), NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 260, 160, 48, 48, hWnd, (HMENU)ID_BUTTON_WEBSITE, hInstance, NULL);
		SendMessage(hButtonOk, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2)));
		SendMessage(hButtonAbout, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON3)));
		SendMessage(hButtonWebsite, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON4)));
		DragAcceptFiles(hWnd, TRUE);
		break;
	case WM_DROPFILES:
		SetValue(szFileName, szNameBuffer, &uFileNum, &uSuccessNum);
		uFileNum = DragFile(hWnd, (HDROP)wParam, szFileName);
		if (uFileNum == 0)
		{
			MessageBox(hWnd, TEXT("不正确的文件数"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
			return 0;
		}
		DisplayFormat(szFileName, szNameBuffer, uFileNum);
		SetWindowText(hEditBox, szNameBuffer);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_OK:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				SetValue(szFileName, szNameBuffer, &uFileNum, &uSuccessNum);
				GetWindowText(hEditBox, szNameBuffer, MAX_DRAG_FILE*MAX_PATH);
				uFileNum = GetTextBox(szNameBuffer, szFileName);
				if (uFileNum == 0)
				{
					MessageBox(hWnd, TEXT("不合法的文件名"), TEXT("Error"), MB_OK | MB_ICONERROR);
					return 0;
				}
				uSuccessNum = HashMod(szFileName, uFileNum);
				wsprintf(szDisplayBuffer, TEXT("共修改 %d 个文件, 其中成功 %d 个, 失败 %d 个"), uFileNum, uSuccessNum, uFileNum - uSuccessNum);
				MessageBox(hWnd, szDisplayBuffer, TEXT("Result"), MB_OK | MB_ICONINFORMATION);
			}
			break;
		case ID_BUTTON_ABOUT:
			MessageBox(hWnd, TEXT("Hash Modifier\nChange your File Fingerprint easily!\nVersion: \tv1.0\nAuthor: \t雪峰"), TEXT("About"), MB_OK);
			break;
		case ID_BUTTON_WEBSITE:
			ShellExecute(NULL, TEXT("open"), TEXT("http://www.xsnow.moe/2015/10/23/HashModifier"), NULL, NULL, SW_NORMAL);
			break;
		}
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

UINT DragFile(HWND hWnd, HDROP hDrop, TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH])
{
	UINT uCount, uStatus, uFileNum = 0;
	uFileNum = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	if (uFileNum > MAX_DRAG_FILE)
		return 0;
	for (uCount = 0; uCount < uFileNum; uCount++)
	{
		uStatus = DragQueryFile(hDrop, uCount, szFileName[uCount], MAX_PATH);
		if (!uStatus)
			return 0;
	}
	return uFileNum;
}

BOOL DisplayFormat(TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], UINT uFileNum)
{
	UINT uCount;
	for (uCount = 0; uCount < uFileNum; uCount++)
	{
		wcscat(szNameBuffer, szFileName[uCount]);
		wcscat(szNameBuffer, TEXT("\r\n"));
	}
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

UINT GetTextBox(TCHAR szNameBuffer[MAX_DRAG_FILE * MAX_PATH], TCHAR szFileName[10][MAX_PATH])
{
	TCHAR *temp, *p;
	temp = p = szNameBuffer;
	UINT i, j, uFileNum;
	i = j = 0;
	while (*temp && *temp == TEXT('\r'))
		temp += 2;
	if (*temp == TEXT('\0'))
		return 0;
	else
		uFileNum = 1;
	while (*p != TEXT('\0'))
	{
		if (*p == TEXT('\r'))
		{
			p += 2;
			i++;
			j = 0;
			if (*p)
				uFileNum++;
			continue;
		}
		szFileName[i][j++] = *p++;
	}
	return uFileNum;
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

