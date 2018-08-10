//Snow 2015-10-23 Initial Version
//Snow 2018-08 Version 1.1
#include <Windows.h>
#include "MainFrame.h"
#include "FileOperation.h"
#include "resource.h"

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT iCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	LPTSTR szWndClassName = TEXT("Xfen");
	HWND hWnd;
	MSG Message;

	FILEPATHLIST PathList;
	ZeroMemory(&PathList, sizeof(FILEPATHLIST));
	
	if (FALSE == RegisterWindowClass(hInstance, szWndClassName))
	{
		MessageBox(NULL, TEXT("Register Window Class Fail!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return 0;
	}
	
	hWnd = CreateWindow(szWndClassName, TEXT("Hash Modifier"), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 345, 250, NULL, NULL, hInstance, &PathList);
	if (NULL == hWnd)
	{
		MessageBox(NULL, TEXT("Create Window Fail!"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return 0;
}

BOOL RegisterWindowClass(_In_ HINSTANCE hInstance, _In_ LPTSTR szWndClassName)
{
	WNDCLASS WindowClass;
	WindowClass.hInstance = hInstance;
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WindowClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR));
	WindowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WindowClass.lpszClassName = szWndClassName;
	WindowClass.lpszMenuName = NULL;

	if (RegisterClass(&WindowClass))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	static UINT uFileNum, uSuccessNum;
	static TCHAR szFileName[MAX_DRAG_FILE][MAX_PATH], szNameBuffer[10 * MAX_PATH], szDisplayBuffer[64];
	static HWND hEditBox, hButtonOk, hButtonAbout, hButtonWebsite;

	BOOL bResult;
	static FILEPATHLIST *PathList;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT *Cs = (LPCREATESTRUCT)lParam;
		PathList = (FILEPATHLIST *)Cs->lpCreateParams;
		hEditBox = CreateWindow(TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL, 20, 5, 300, 150, hWnd, (HMENU)ID_EDIT_BOX, Cs->hInstance, NULL);
		hButtonOk = CreateWindow(TEXT("BUTTON"), NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_ICON, 30, 160, 48, 48, hWnd, (HMENU)ID_BUTTON_OK, Cs->hInstance, NULL);
		hButtonAbout = CreateWindow(TEXT("BUTTON"), NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 145, 160, 48, 48, hWnd, (HMENU)ID_BUTTON_ABOUT, Cs->hInstance, NULL);
		hButtonWebsite = CreateWindow(TEXT("BUTTON"), NULL, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON, 260, 160, 48, 48, hWnd, (HMENU)ID_BUTTON_WEBSITE, Cs->hInstance, NULL);
		SendMessage(hButtonOk, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(Cs->hInstance, MAKEINTRESOURCE(IDI_ICON2)));
		SendMessage(hButtonAbout, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(Cs->hInstance, MAKEINTRESOURCE(IDI_ICON3)));
		SendMessage(hButtonWebsite, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(Cs->hInstance, MAKEINTRESOURCE(IDI_ICON4)));
		DragAcceptFiles(hWnd, TRUE);
		break;
	}

	case WM_DROPFILES:
		bResult = RetrieveDragFilePath((HDROP)wParam, PathList);
		if (FALSE == bResult)
		{
			MessageBox(hWnd, TEXT("获取拖入文件路径失败"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
			return 0;
		}
		DisplayPathListInEditBox(hEditBox, PathList);
		return 0;

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			switch (LOWORD(wParam))
			{
			case ID_BUTTON_OK:
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
				break;
			case ID_BUTTON_ABOUT:
				MessageBox(hWnd, TEXT("Hash Modifier\nChange your File Fingerprint easily!\nVersion: \tv1.1\nAuthor: \t雪峰"), TEXT("About"), MB_OK);
				break;
			case ID_BUTTON_WEBSITE:
				ShellExecute(NULL, TEXT("open"), TEXT("http://www.xsnow.moe/2015/10/23/HashModifier"), NULL, NULL, SW_NORMAL);
				break;
			}
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL RetrieveDragFilePath(_In_ HDROP hDrop, _Inout_ FILEPATHLIST *PathList)
{
	if (NULL == hDrop || NULL == PathList)
	{
		return FALSE;
	}
	UINT uDragFileNum = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	UINT uCurrentNum = 0;
	TCHAR szBuffer[MAX_PATH];

	while (uCurrentNum < uDragFileNum)
	{
		UINT uReturnValue = DragQueryFile(hDrop, uCurrentNum, szBuffer, MAX_PATH);
		if (uReturnValue)
		{
			AppendPathToFilePathList(szBuffer, PathList);
		}
		else
		{
			DestroyFilePathList(PathList);
			return FALSE;
		}
		uCurrentNum++;
	}

	return TRUE;
}

VOID DisplayPathListInEditBox(_In_ HWND hEditBox, _Inout_ FILEPATHLIST *PathList)
{
	if (NULL == hEditBox || NULL == PathList)
	{
		return;
	}
	UINT uBytesRequiredForMerge = CalcBytesRequiredForMergeFilePathList(PathList);
	if (0 == uBytesRequiredForMerge)
	{
		return;
	}

	LPTSTR szMultilineString = LocalAlloc(LMEM_FIXED, uBytesRequiredForMerge);
	if (NULL == szMultilineString)
	{
		return;
	}

	MergeFilePathListIntoOneMultiLineString(szMultilineString, uBytesRequiredForMerge, PathList);
	SetWindowText(hEditBox, szMultilineString);
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

