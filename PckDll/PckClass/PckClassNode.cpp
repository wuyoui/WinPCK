#include "PckClassNode.h"

template <typename T>
_inline void strpathcpy(T * dst, T * &src)
{
	while((*dst++ = *src) && '\\' != *++src && '/' != *src)
		;
}
#pragma region �ຯ��

CPckClassNode::CPckClassNode()
{
	m_lpRootNode = &m_PckAllInfo.cRootNode;
}

CPckClassNode::~CPckClassNode()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

#pragma endregion
#pragma region ParseIndexTableToNode

void CPckClassNode::ParseIndexTableToNode(LPPCKINDEXTABLE lpMainIndexTable)
{
	LPPCKINDEXTABLE lpPckIndexTable = lpMainIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//������ͱ�ʶ
		lpPckIndexTable->entryType = PCK_ENTRY_TYPE_INDEX;
		//����Ŀ¼
		AddFileToNode(lpPckIndexTable);
		++lpPckIndexTable;
	}
	
	//����һ��root�ڵ��µ�entryType���ϱ�ǩ��ʾ����
	if(NULL != m_PckAllInfo.cRootNode.child)
		m_PckAllInfo.cRootNode.child->entryType |= PCK_ENTRY_TYPE_ROOT;
}

/********************************
*
*���ļ�����Ŀ¼(\��/����֣���ӵ�Ŀ¼�ڵ���ȥ
*
*
**********************************/

BOOL CPckClassNode::AddFileToNode(LPPCKINDEXTABLE lpPckIndexTable)
{
	LPPCK_PATH_NODE lpChildNode = &m_PckAllInfo.cRootNode;
	LPPCK_PATH_NODE	lpFirstNode = NULL;

	wchar_t			*lpszFilename = lpPckIndexTable->cFileIndex.szwFilename;

	do {
		//�˽ڵ��»�û���ļ�����һ���²����Ľڵ㣩���������".."
		if(NULL == (lpChildNode->child)) {

			lpChildNode->child = (LPPCK_PATH_NODE)m_NodeMemPool.Alloc(sizeof(PCK_PATH_NODE));

			lpChildNode->child->parent = lpChildNode;
			lpChildNode->child->parentfirst = lpFirstNode;

			//ʹ��nNameSizeAnsi ��..Ŀ¼��¼��Ŀ¼·������gfx\�µ�..Ŀ¼���ĳ��ȣ�ansi��
			if(NULL != lpFirstNode)
				lpChildNode->child->nNameSizeAnsi = (lpFirstNode->nNameSizeAnsi + lpChildNode->nNameSizeAnsi + 1);

			//���..Ŀ¼
			memcpy(lpChildNode->child->szName, L"..", wcslen(L"..") * sizeof(wchar_t));

			lpChildNode->child->entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER | PCK_ENTRY_TYPE_DOTDOT;
		}

		lpFirstNode = lpChildNode = lpChildNode->child;

		//����Ŀ¼�㣨����\\��

		wchar_t	szToFind[MAX_PATH_PCK_260] = { 0 };
		strpathcpy(szToFind, lpszFilename);

		do {
			//��һ��Ŀ¼(next '\\')
			//�˼�Ŀ¼���½ڵ��˽ڵ�����ЧĿ¼�ڵ�
			if((NULL == lpChildNode->lpPckIndexTable) || (!lpChildNode->lpPckIndexTable->isInvalid)) {

				if (0 == wcscmp(lpChildNode->szName, szToFind)) {
					//��������ļ����У���

					//�����ظ����ļ�������ǰһ���ظ����ļ�Ϊ��Ч
					if(0 == *lpszFilename)
						lpChildNode->lpPckIndexTable->isInvalid = TRUE;

					break;
				}

				//�˼��ڵ��ѱ����꣬û���ظ��ģ�����½ڵ�
				if(NULL == lpChildNode->next) {

					//����ļ����У�
					lpChildNode->next = (LPPCK_PATH_NODE)m_NodeMemPool.Alloc(sizeof(PCK_PATH_NODE));
					lpChildNode = lpChildNode->next;

					lpChildNode->parent = lpFirstNode->parent;

					lpChildNode->entryType = PCK_ENTRY_TYPE_NODE | PCK_ENTRY_TYPE_FOLDER;

					wcscpy(lpChildNode->szName, szToFind);
					lpChildNode->nNameSizeAnsi = CPckClassCodepage::PckFilenameCode2Ansi(lpChildNode->szName, NULL, 0);
					lpChildNode->nMaxNameSizeAnsi = MAX_PATH_PCK_256;

					//ͳ�Ƹ��ļ��е����ļ�����
					if(0 != *lpszFilename) {
						LPPCK_PATH_NODE	lpAddDirCount = lpFirstNode;
						do {
							++(lpAddDirCount->dwDirsCount);
							lpAddDirCount = lpAddDirCount->parentfirst;

						} while(NULL != lpAddDirCount);

					}

					break;
				}
			}
			else {
				throw MyException("�����ϲ�������ЧĿ¼�ڵ�!");
				//������
			}

			//ͬ��Ŀ¼����һ���ڵ�
			lpChildNode = lpChildNode->next;

		} while(1);

		//���ļ�������ͳ��
		++(lpFirstNode->dwFilesCount);
		lpFirstNode->qdwDirCipherTextSize += lpPckIndexTable->cFileIndex.dwFileCipherTextSize;
		lpFirstNode->qdwDirClearTextSize += lpPckIndexTable->cFileIndex.dwFileClearTextSize;

		if (lpChildNode->nMaxNameSizeAnsi > lpPckIndexTable->nFilelenLeftBytes) {
			lpChildNode->nMaxNameSizeAnsi = lpPckIndexTable->nFilelenLeftBytes;
		}

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	lpChildNode->lpPckIndexTable = lpPckIndexTable;
	lpChildNode->entryType = PCK_ENTRY_TYPE_NODE;

	return TRUE;

}
#pragma endregion

#pragma region FindFileNode
const PCK_PATH_NODE* CPckClassNode::FindFileNode(const PCK_PATH_NODE* lpBaseNode, wchar_t* lpszFile)
{
	if ((NULL == lpBaseNode) || (NULL == lpBaseNode->child))
		return NULL;

	const PCK_PATH_NODE* lpChildNode = lpBaseNode->child;

	wchar_t			szFilename[MAX_PATH];
	wcscpy_s(szFilename, lpszFile);

	wchar_t			*lpszFilename = szFilename;

	if(NULL == lpChildNode->szName)
		return NULL;

	do {
		wchar_t	szToFind[MAX_PATH_PCK_260] = { 0 };
		strpathcpy(szToFind, lpszFilename);

		do {

			if(0 == wcscmp(lpChildNode->szName, szToFind)) {

				if(NULL == lpChildNode->child && 0 == *lpszFilename)return lpChildNode;

				if((NULL == lpChildNode->child && (TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)) || (NULL != lpChildNode->child && 0 == *lpszFilename)) {
					return (LPPCK_PATH_NODE)INVALID_NODE;
				}

				break;
			}

			if(NULL == lpChildNode->next) {
				return NULL;
			}
			lpChildNode = lpChildNode->next;

		} while(1);

		lpChildNode = lpChildNode->child;

		if(TEXT('\\') == *lpszFilename || TEXT('/') == *lpszFilename)
			++lpszFilename;

	} while(*lpszFilename);

	return NULL;

}

#pragma endregion
#pragma region DeleteNode

VOID CPckClassNode::DeleteNode(LPPCKINDEXTABLE lpIndex)
{
	lpIndex->isInvalid = TRUE;
}

VOID CPckClassNode::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	lpNode = lpNode->child->next;

	while(NULL != lpNode) {
		if(NULL == lpNode->child) {
			DeleteNode(lpNode->lpPckIndexTable);
		} else {
			DeleteNode(lpNode);
		}

		lpNode = lpNode->next;
	}
}

#pragma endregion

#pragma region RenameNode

BOOL CPckClassNode::RenameNodeEnum(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{

	lpNode = lpNode->child->next;

	while(NULL != lpNode) {

		if(NULL == lpNode->child) {

			if(!RenameNode(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		} else {
			if(!RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceString, lenrs, lenrp))
				return FALSE;
		}

		lpNode = lpNode->next;
	}
	return TRUE;
}

BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, size_t lenNodeRes, char* lpszReplaceString, size_t lenrs, size_t lenrp)
{
	//if(lenrs >= (MAX_PATH_PCK_260 - strlen(lpNode->lpPckIndexTable->cFileIndex.szFilename + lenNodeRes - 2)))return FALSE;
	char	szTemp[MAX_PATH_PCK_260] = { 0 };
	char	*lpszReplacePos = lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp - lenNodeRes;

	//DebugA("lpszReplaceString = %s \r\nlenNodeRes = %d\r\nlenrs = %d\r\nlenrp = %d\r\n===============================\r\n",
	//		lpszReplaceString, lenNodeRes, lenrs, lenrp);
	memcpy(szTemp, lpNode->lpPckIndexTable->cFileIndex.szFilename + lenrp, MAX_PATH_PCK_260 - lenrp);
	memcpy(lpszReplacePos, lpszReplaceString, lenrs);
	memcpy(lpszReplacePos + lenrs, szTemp, MAX_PATH_PCK_260 - lenrp - lenrs + lenNodeRes);

	return TRUE;
}

//public
BOOL CPckClassNode::RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString)
{
	//�����ļ���Ϊa\b\c\d.exe
	//���ڵ�Ϊc
	size_t	lenNodeRes, lenrs, lenrp;
	char	lpszReplaceStringAnsi[MAX_PATH_PCK_260];

	lenrs = CPckClassCodepage::PckFilenameCode2Ansi(lpszReplaceString, lpszReplaceStringAnsi, sizeof(lpszReplaceStringAnsi));

	lenNodeRes = lpNode->nNameSizeAnsi;

	size_t nAllowedMaxSize = lpNode->nMaxNameSizeAnsi + lenNodeRes;

	if (nAllowedMaxSize < lenrs)
		return FALSE;

	//ȡ��"a\b\"
	lenrp = lpNode->child->nNameSizeAnsi - 1;

	return RenameNodeEnum(lpNode, lenNodeRes, lpszReplaceStringAnsi, lenrs, lenrp);

}


BOOL CPckClassNode::RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString)
{
	//���Ŀ¼���ģʽ�µ��ļ������޸�
	int		nBytesReadayToWrite;
	char	*lpszPosToWrite;	//�ļ�����ַ������lpszFileTitle

	lpszPosToWrite = lpNode->lpPckIndexTable->cFileIndex.szFilename + lpNode->lpPckIndexTable->nFilelenBytes - lpNode->nNameSizeAnsi;
	nBytesReadayToWrite = lpNode->nMaxNameSizeAnsi + lpNode->nNameSizeAnsi;

	//ת��Ϊansi����鳤��
	char szReplaceStringAnsi[MAX_PATH_PCK_260];

	size_t nLenOfReplaceString = CPckClassCodepage::PckFilenameCode2Ansi(lpszReplaceString, szReplaceStringAnsi, MAX_PATH_PCK_260);

	if (nBytesReadayToWrite < nLenOfReplaceString)
		return FALSE;

	memset(lpszPosToWrite, 0, nBytesReadayToWrite);
	strcpy(lpszPosToWrite, szReplaceStringAnsi);
	return TRUE;
}

BOOL CPckClassNode::RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString)
{
	//ת��Ϊansi����鳤��
	char szReplaceStringAnsi[MAX_PATH_PCK_260];

	size_t nLenOfReplaceString = CPckClassCodepage::PckFilenameCode2Ansi(lpszReplaceString, szReplaceStringAnsi, MAX_PATH_PCK_260);

	if (MAX_PATH_PCK_256 < nLenOfReplaceString)
		return FALSE;

	memset(lpIndex->cFileIndex.szFilename, 0, MAX_PATH_PCK_260);
	strcpy(lpIndex->cFileIndex.szFilename, szReplaceStringAnsi);
	return TRUE;
}

BOOL CPckClassNode::GetCurrentNodeString(wchar_t *szCurrentNodePathString, const PCK_PATH_NODE* lpNode)
{
	if ((NULL == lpNode) || (PCK_ENTRY_TYPE_ROOT == (PCK_ENTRY_TYPE_ROOT & lpNode->entryType))) {
		*szCurrentNodePathString = 0;
		return TRUE;
	}

	int entry_type = lpNode->entryType;

	if (PCK_ENTRY_TYPE_DOTDOT == (PCK_ENTRY_TYPE_DOTDOT & entry_type)) {

		if ((NULL == lpNode->parentfirst) || (NULL == lpNode->parentfirst->parent)) {
			*szCurrentNodePathString = 0;
			return TRUE;
		}

		lpNode = lpNode->parentfirst->parent;

	}
	entry_type = lpNode->entryType;

	if (PCK_ENTRY_TYPE_ROOT == (PCK_ENTRY_TYPE_ROOT & lpNode->entryType)) {
		*szCurrentNodePathString = 0;
		return TRUE;
	}

	GetCurrentNodeString(szCurrentNodePathString, lpNode->parent);
	wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, lpNode->szName);

	if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entry_type))
		wcscat_s(szCurrentNodePathString, MAX_PATH_PCK_260, L"\\");

	return TRUE;
}

#pragma endregion


BOOL CPckClassNode::FindDuplicateNodeFromFileList(const PCK_PATH_NODE* lpNodeToInsertPtr, DWORD &_in_out_DuplicateFileCount)
{
	vector<FILES_TO_COMPRESS> *lpFilesList = m_PckAllInfo.lpFilesToBeAdded;
	size_t sizeOfFileList = lpFilesList->size();

	for(size_t i=0;i<sizeOfFileList;i++){

		FILES_TO_COMPRESS *lpfirstFile = &(*lpFilesList)[i];
		const PCK_PATH_NODE* lpDuplicateNode = FindFileNode(lpNodeToInsertPtr, lpfirstFile->szwFilename + lpfirstFile->nFileTitleLen);

		if(INVALID_NODE == (int)lpDuplicateNode) {
			Logger.w(TEXT_ERROR_DUP_FOLDER_FILE);
			assert(FALSE);
			return FALSE;
		}

		if(NULL != lpDuplicateNode) {
			lpfirstFile->samePtr = lpDuplicateNode->lpPckIndexTable;
			lpDuplicateNode->lpPckIndexTable->isInvalid = TRUE;
			++_in_out_DuplicateFileCount;
		}

	}
	return TRUE;
}