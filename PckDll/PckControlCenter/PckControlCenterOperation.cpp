
//////////////////////////////////////////////////////////////////////
// PckControlCenterOperation.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
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
#include "ZupClass.h"
#include "PckClassZlib.h"
#include "PckClassLog.h"
#include "PckClassRebuildFilter.h"


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);
}

#pragma region �򿪹ر��ļ�

FMTPCK	CPckControlCenter::GetPckTypeFromFilename(const wchar_t * lpszFile)
{
	size_t nFileLength = wcsnlen(lpszFile, MAX_PATH);

	if(0 == wcsicmp(lpszFile + nFileLength - 4, L".pck")) {
		return FMTPCK_PCK;
	} else if(0 == wcsicmp(lpszFile + nFileLength - 4, L".zup")) {
		return FMTPCK_ZUP;
	}
	return FMTPCK_UNKNOWN;
}

BOOL CPckControlCenter::Open(const wchar_t * lpszFile)
{
	//�ж��ļ���ʽ
	FMTPCK emunFileFormat = GetPckTypeFromFilename(lpszFile);

	while(1) {

		Close();
		switch(emunFileFormat) {
		case FMTPCK_ZUP:
			m_lpClassPck = new CZupClass(&cParams);
			break;
		case FMTPCK_PCK:
		default:
			m_lpClassPck = new CPckClass(&cParams);
			break;
		}

		Logger.i(UCSTEXT(TEXT_LOG_OPENFILE), lpszFile);

		if(m_lpClassPck->Init(lpszFile)) {

			m_emunFileFormat = emunFileFormat;

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			//�򿪳ɹ���ˢ�±���
			pFeedbackCallBack(pTag, PCK_FILE_OPEN_SUCESS, NULL, (ssize_t)(m_lpClassPck->GetPckVersion()->name));
			return TRUE;

		} else {

			return FALSE;
		}
	}
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck) {

		if(IsValidPck())
			Logger.i(TEXT_LOG_CLOSEFILE);

		delete m_lpClassPck;
		m_lpClassPck = NULL;

		m_lpPckRootNode = NULL;
	}

	m_emunFileFormat = FMTPCK_UNKNOWN;

	//��ղ���
	Reset();

	//�ر��ļ���ˢ�±���
	pFeedbackCallBack(pTag, PCK_FILE_CLOSE, NULL, NULL);
}

#pragma endregion

#pragma region �������ڵ�

BOOL CPckControlCenter::RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	int entryType = lpFileEntry->entryType;
	if (PCK_ENTRY_TYPE_NODE == entryType) {

		return m_lpClassPck->RenameIndex((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & entryType)) {

		return m_lpClassPck->RenameNode((LPPCK_PATH_NODE)lpFileEntry, lpszReplaceString);
	}
	else if (PCK_ENTRY_TYPE_INDEX == entryType)
	{
		return m_lpClassPck->RenameIndex((LPPCKINDEXTABLE)lpFileEntry, lpszReplaceString);
	}
	else {
		return FALSE;
	}
}
BOOL CPckControlCenter::RenameSubmit()
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RenameFilename();
}

#pragma endregion

#pragma region Ԥ����ѹ�ļ�

//Ԥ���ļ�
BOOL CPckControlCenter::GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	const PCKINDEXTABLE* lpPckFileIndexTable = NULL;
	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		lpPckFileIndexTable = (LPPCKINDEXTABLE)lpFileEntry;
	else
		lpPckFileIndexTable = ((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable;

	return m_lpClassPck->GetSingleFileData(lpPckFileIndexTable, buffer, sizeOfBuffer);
}

//��ѹ�ļ�
BOOL CPckControlCenter::ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractFiles(lpFileEntryArray, nEntryCount, lpszDestDirectory);
}

BOOL CPckControlCenter::ExtractAllFiles(LPCWSTR lpszDestDirectory)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->ExtractAllFiles(lpszDestDirectory);
}
#pragma endregion

#pragma region �ؽ�pck�ļ�
//�ؽ�pck�ļ�
BOOL CPckControlCenter::TestScript(LPCWSTR lpszScriptFile)
{
	CPckClassRebuildFilter cScriptFilter;

	return cScriptFilter.TestScript(lpszScriptFile);
}

BOOL CPckControlCenter::RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->RebuildPckFile(lpszScriptFile, szRebuildPckFile, bUseRecompress);
}

#pragma endregion

#pragma region ��Ϸ����
BOOL CPckControlCenter::StripPck(LPCWSTR lpszStripedPckFile, int flag)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	return m_lpClassPck->StripPck(lpszStripedPckFile, flag);
	
}

#pragma endregion

#pragma region ����ļ��б����ѹ��ʱʹ�õ�vector����
void CPckControlCenter::StringArrayReset()
{
	lpszFilePathToAdd.clear();
}

void CPckControlCenter::StringArrayAppend(LPCWSTR lpszFilePath)
{
	lpszFilePathToAdd.push_back(lpszFilePath);
}

#pragma endregion

#pragma region �½�/����pck�ļ�

BOOL CPckControlCenter::UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry)
{
	if (NULL == m_lpClassPck)
		return FALSE;

	BOOL rtn = FALSE;

	if (0 != lpszFilePathToAdd.size()) {
		rtn = m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePathToAdd, (const PCK_PATH_NODE*)lpFileEntry);
		StringArrayReset();
	}
	return rtn;
}

#pragma region ɾ���ڵ�
//ɾ��һ���ڵ�
BOOL CPckControlCenter::DeleteEntry(LPCENTRY lpFileEntry)
{
	if ((NULL == m_lpClassPck) || (NULL == lpFileEntry))
		return FALSE;

	if (PCK_ENTRY_TYPE_INDEX == lpFileEntry->entryType)
		m_lpClassPck->DeleteNode((LPPCKINDEXTABLE)lpFileEntry);
	else if (PCK_ENTRY_TYPE_FOLDER == (PCK_ENTRY_TYPE_FOLDER & lpFileEntry->entryType))
		m_lpClassPck->DeleteNode((LPPCK_PATH_NODE)lpFileEntry);
	else
		m_lpClassPck->DeleteNode(((LPPCK_PATH_NODE)lpFileEntry)->lpPckIndexTable);
	return TRUE;
}

//�ύ
BOOL CPckControlCenter::DeleteEntrySubmit()
{
	return RenameSubmit();
}

#pragma endregion


#pragma region �汾���

BOOL CPckControlCenter::SetPckVersion(int verID)
{
	return m_lpClassPck->SetSavePckVersion(verID);
}

int	CPckControlCenter::GetPckVersion()
{
	return m_lpClassPck->GetPckVersion()->id;
}

//��ȡ��ǰ��������
LPCWSTR	CPckControlCenter::GetCurrentVersionName()
{
	if (NULL == m_lpClassPck)
		return NULL;

	return m_lpClassPck->GetPckVersion()->name;
}

uint32_t	CPckControlCenter::GetVersionCount()
{
	return CPckClassVersionDetect::GetPckVersionCount();
}

LPCWSTR	CPckControlCenter::GetVersionNameById(int verID)
{
	return CPckClassVersionDetect::GetPckVersionNameById(verID);
}

int CPckControlCenter::AddVersionAlgorithmId(int AlgorithmId, int Version)
{
	return CPckClassVersionDetect::AddPckVersion(AlgorithmId, Version);
}

#pragma endregion
