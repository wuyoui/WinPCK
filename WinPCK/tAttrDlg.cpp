//////////////////////////////////////////////////////////////////////
// tAttrDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"
#include <shlwapi.h>

TAttrDlg::TAttrDlg(const PCK_UNIFIED_FILE_ENTRY *_lpPckInfo, wchar_t *_lpszPath, TWin *_win) :
	TDlg(IDD_DIALOG_ATTR, _win),
	lpPckInfo(_lpPckInfo),
	lpszPath(_lpszPath)
{
}

//TAttrDlg::~TAttrDlg()
//{
//}

BOOL TAttrDlg::EvCreate(LPARAM lParam)
{
#define PRINTF_BYTESIZE	32
#define PRINTF_SIZE		80

	wchar_t	*lpszFilename;
	const wchar_t	*lpszwFilename;
	wchar_t	szPrintf[PRINTF_SIZE];
	wchar_t	szPrintfBytesize[PRINTF_BYTESIZE];
	LPCWSTR	lpszFormat = L"%s (%llu �ֽ�)";
	LPCWSTR	lpszFileFormat = L"%s (%u �ֽ�)";

	if (PCK_ENTRY_TYPE_INDEX == lpPckInfo->entryType) {
		wchar_t	szFilename[MAX_PATH_PCK_260];

		wcscpy(szFilename, lpPckInfo->szName);

		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"�ļ�");

		if (NULL != (lpszFilename = wcsrchr(szFilename, L'\\'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextW(IDC_EDIT_ATTR_PATH, szFilename);

			lpszwFilename = lpszFilename;

		}
		else if (NULL != (lpszFilename = wcsrchr(szFilename, L'/'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextW(IDC_EDIT_ATTR_PATH, szFilename);

			lpszwFilename = lpszFilename;

		}
		else {

			lpszwFilename = lpPckInfo->szName;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, "");
		}

		SetDlgItemTextW(IDC_EDIT_ATTR_NAME, lpszwFilename);
	}
	else {
		SetDlgItemTextW(IDC_EDIT_ATTR_NAME, lpszwFilename = lpPckInfo->szName);
	}

	SetDlgItemTextW(IDC_EDIT_ATTR_PATH, lpszPath);

	uint64_t qdwDirClearTextSize = pck_getFileSizeInEntry(lpPckInfo);
	uint64_t qdwDirCipherTextSize = pck_getCompressedSizeInEntry(lpPckInfo);

	//ѹ����С
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
		StrFormatByteSizeW(qdwDirCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirCipherTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_CIPHER, szPrintf);


	//ʵ�ʴ�С
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
		StrFormatByteSizeW(qdwDirClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirClearTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_SIZE, szPrintf);

	//ѹ����
	if (0 == qdwDirClearTextSize)
		SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
	else {
		swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", qdwDirCipherTextSize / (double)qdwDirClearTextSize * 100);
		SetDlgItemTextW(IDC_EDIT_ATTR_CMPR, szPrintf);
	}

	if (PCK_ENTRY_TYPE_FOLDER != (PCK_ENTRY_TYPE_FOLDER & lpPckInfo->entryType))//�ļ�
	{
		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"�ļ�");

		//����
		SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

		//ƫ�Ƶ�ַ
		swprintf_s(szPrintf, PRINTF_SIZE, L"%llu (0x%016llX)", pck_getFileOffset(lpPckInfo), pck_getFileOffset(lpPckInfo));
		SetDlgItemTextW(IDC_EDIT_ATTR_ADDR, szPrintf);
	}
	else {

		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"�ļ���");

		//����
		swprintf_s(szPrintf, PRINTF_SIZE, L"%u ���ļ���%u ���ļ���", pck_getFilesCountInEntry(lpPckInfo), pck_getFoldersCountInEntry(lpPckInfo));
		SetDlgItemTextW(IDC_EDIT_ATTR_FILECOUNT, szPrintf);

		//ƫ�Ƶ�ַ
		SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, "-");

	}

	//�ļ�����Ϣ
	const PCK_UNIFIED_FILE_ENTRY* lpRootNode = pck_getRootNode();

	qdwDirClearTextSize = pck_getFileSizeInEntry(lpRootNode);
	qdwDirCipherTextSize = pck_getCompressedSizeInEntry(lpRootNode);

	//�ļ��ܴ�С
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qdwDirClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirClearTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_ALLSIZE, szPrintf);

	//ѹ������С
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qdwDirCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qdwDirCipherTextSize);
	SetDlgItemTextW(IDC_EDIT_ATTR_PCKSIZE, szPrintf);

	//ѹ����
	swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", qdwDirCipherTextSize / (float)qdwDirClearTextSize * 100.0);
	SetDlgItemTextW(IDC_EDIT_ATTR_PCKCMPR, szPrintf);

	//����������
	uint64_t qwPckFileSize = pck_file_redundancy_data_size();
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qwPckFileSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qwPckFileSize);
	SetDlgItemTextW(IDC_EDIT_ATTR_DIRT, szPrintf);


	//��������
	TCHAR	szTitle[MAX_PATH];
	_stprintf_s(szTitle, MAX_PATH, TEXT("%s ����"), lpszwFilename);
	SetWindowText(szTitle);

	Show();

	return	FALSE;

#undef PRINTF_SIZE
#undef PRINTF_BYTESIZE
}

BOOL TAttrDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
