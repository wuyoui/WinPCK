//////////////////////////////////////////////////////////////////////
// PckControlCenterAttribute.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "PckClass.h"

#pragma region pck�ļ�����

BOOL CPckControlCenter::IsValidPck()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->isFileLoaded();
}

//��ȡ�ļ���
uint32_t CPckControlCenter::GetPckFileCount()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->GetPckFileCount();
}

QWORD CPckControlCenter::GetPckSize()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->GetPckSize();
}

QWORD CPckControlCenter::GetPckDataAreaSize()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->GetPckDataAreaSize();
}

QWORD CPckControlCenter::GetPckRedundancyDataSize()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->GetPckRedundancyDataSize();
}

QWORD CPckControlCenter::GetFileSizeInEntry(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		return ((LPPCKINDEXTABLE)lpFileEntry)->cFileIndex.dwFileClearTextSize;
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		return ((LPPCK_PATH_NODE)lpFileEntry)->child->qdwDirClearTextSize;
	else
		return ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable->cFileIndex.dwFileClearTextSize;
}

QWORD CPckControlCenter::GetCompressedSizeInEntry(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		return ((LPPCKINDEXTABLE)lpFileEntry)->cFileIndex.dwFileCipherTextSize;
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		return ((LPPCK_PATH_NODE)lpFileEntry)->child->qdwDirCipherTextSize;
	else
		return ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
}

uint32_t CPckControlCenter::GetFoldersCountInEntry(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		return ((LPPCK_PATH_NODE)lpFileEntry)->child->dwDirsCount;
	else
		return 0;
}

uint32_t CPckControlCenter::GetFilesCountInEntry(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		return ((LPPCK_PATH_NODE)lpFileEntry)->child->dwFilesCount;
	else
		return 0;
}

size_t CPckControlCenter::GetFilelenBytesOfEntry(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		return ((LPPCKINDEXTABLE)lpFileEntry)->nFilelenBytes;
	else
		return ((LPPCK_PATH_NODE)lpFileEntry)->nNameSizeAnsi;
}

size_t CPckControlCenter::GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		return ((LPPCKINDEXTABLE)lpFileEntry)->nFilelenLeftBytes;
	else
		return ((LPPCK_PATH_NODE)lpFileEntry)->nMaxNameSizeAnsi;
}

uint64_t CPckControlCenter::GetFileOffset(LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return -1;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		return ((LPPCKINDEXTABLE)lpFileEntry)->cFileIndex.dwAddressOffset;
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		return 0;
	else
		return ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable->cFileIndex.dwAddressOffset;
}

const char*	CPckControlCenter::GetAdditionalInfo()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->GetAdditionalInfo();
}

uint32_t CPckControlCenter::GetAdditionalInfoMaxSize()
{
	return PCK_ADDITIONAL_INFO_SIZE;
}

BOOL CPckControlCenter::SetAdditionalInfo(LPCSTR lpszAdditionalInfo)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->SetAdditionalInfo(lpszAdditionalInfo);
}

BOOL CPckControlCenter::isSupportAddFileToPck()
{
	switch (m_emunFileFormat) {
	case FMTPCK_PCK:
		return TRUE;
		break;

	case FMTPCK_ZUP:
		return FALSE;
		break;
	}
	return FALSE;
}

#pragma endregion



#pragma region �ڵ����Բ���

LPCENTRY CPckControlCenter::GetRootNode()
{
	return (LPENTRY)m_lpPckRootNode;
}

BOOL CPckControlCenter::GetCurrentNodeString(LPWSTR szCurrentNodePathString, LPCENTRY lpFileEntry)
{
	if (NULL == lpFileEntry)
		return FALSE;

	if (PCK_ENTRY_TYPE_INDEX != lpFileEntry->entryType) {
		const PCK_PATH_NODE* lpNode = (LPPCK_PATH_NODE)lpFileEntry;
		return CPckClassNode::GetCurrentNodeString(szCurrentNodePathString, lpNode);
	}
	else {
		return FALSE;
	}
}

LPCENTRY CPckControlCenter::GetFileEntryByPath(LPCWSTR _in_szCurrentNodePathString)
{
	const PCK_PATH_NODE* lpCurrentNode = (PCK_PATH_NODE*)GetRootNode();
	const PCK_PATH_NODE* lpCurrentNodeToFind = lpCurrentNode;

	if (NULL == _in_szCurrentNodePathString)
		return NULL;

	if (0 == *_in_szCurrentNodePathString)
		return (LPPCK_UNIFIED_FILE_ENTRY)lpCurrentNode;

	wchar_t szPath2Parse[MAX_PATH];
	wchar_t *lpszPaths[MAX_PATH] = { NULL };
	wcscpy_s(szPath2Parse, _in_szCurrentNodePathString);

	szPath2Parse[MAX_PATH - 1] = 0;
	wchar_t *lpszSearch = szPath2Parse;
	int nDirCount = 0;

	if ('\\' == *lpszSearch)
		lpszSearch++;

	lpszPaths[nDirCount] = lpszSearch;
	++nDirCount;

	for (int i = 0; i < MAX_PATH; i++) {

		if (0 == *lpszSearch)
			break;
		else if ('\\' == *lpszSearch)
		{
			*lpszSearch = 0;
			if (0 != *(lpszSearch + 1)) {
				lpszPaths[nDirCount] = lpszSearch + 1;
				++nDirCount;
			}
		}
		lpszSearch++;
	}

	wchar_t	**lpCurrentDir = lpszPaths;

	for (int i = 0; i < nDirCount; i++) {

		if (0 == **lpCurrentDir)return (LPPCK_UNIFIED_FILE_ENTRY)lpCurrentNode;

		//������ļ���
		if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpCurrentNodeToFind->entryType)) {
			lpCurrentNode = lpCurrentNodeToFind = lpCurrentNodeToFind->child;
		}
		else {
			return NULL;
		}

		BOOL isDirFound = FALSE;

		while (NULL != lpCurrentNodeToFind) {

			if (0 == wcscmp(lpCurrentNodeToFind->szName, *lpCurrentDir)) {

				lpCurrentNode = lpCurrentNodeToFind;
				isDirFound = TRUE;
				break;
			}

			lpCurrentNodeToFind = lpCurrentNodeToFind->next;
		}

		if (!isDirFound) {
#if PCK_DEBUG_OUTPUT
			printf("node not found\n");
#endif
			return NULL;
			//return (LPPCK_UNIFIED_FILE_ENTRY)lpCurrentNode;
		}

		lpCurrentDir++;

	}
	return (LPPCK_UNIFIED_FILE_ENTRY)lpCurrentNode;
}

#pragma endregion

