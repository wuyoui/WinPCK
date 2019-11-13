#include "PckClassWriteOperator.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

#include <functional>

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassWriteOperator::CPckClassWriteOperator()
{}

CPckClassWriteOperator::~CPckClassWriteOperator()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

/********************************
*
*��������
*
********************/

BOOL CPckClassWriteOperator::RebuildPckFile(const wchar_t * lpszScriptFile, const wchar_t * szRebuildPckFile, BOOL bUseRecompress)
{
	CPckClassRebuildFilter cScriptFilter;

	if ((nullptr != lpszScriptFile) && (0 != *lpszScriptFile)) 
		cScriptFilter.ApplyScript(lpszScriptFile, &m_PckAllInfo.cRootNode);

	return bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);
}

BOOL CPckClassWriteOperator::StripPck(const wchar_t * lpszStripedPckFile, int flag)
{
	//���ȹ���*\textures\*.dds
	CPckClassRebuildFilter cScriptFilter;

	if (PCK_STRIP_DDS & flag) {
		
		cScriptFilter.StripModelTexture(
			m_PckAllInfo.lpPckIndexTable, 
			m_PckAllInfo.dwFileCount,
			m_PckAllInfo.cRootNode.child,
			m_PckAllInfo.szFileTitle
		);
	}
	return RecompressPckFile(lpszStripedPckFile, flag);
}

BOOL CPckClassWriteOperator::RebuildPckFile(const wchar_t * szRebuildPckFile)
{

	Logger.i(TEXT_LOG_REBUILD);

	
	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwValidFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	//����ͷ��βʱ��Ҫ�Ĳ���
	PCK_ALL_INFOS		pckAllInfo;
	//����ͷ��βʱ��Ҫ�Ĳ���
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	wcscpy_s(pckAllInfo.szNewFilename, szRebuildPckFile);

	//�̱߳��
	SetThreadFlag(TRUE);

	//���ý����������ֵ
	SetParams_ProgressUpper(dwValidFileCount);

	//��Դ�ļ� 
	CMapViewFileMultiPckRead	cFileRead;
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename)) 
		return FALSE;

	//��Ŀ���ļ� 
	//�����Ǵ���һ���ļ������������ؽ�����ļ�
	CMapViewFileMultiPckWrite	cFileWrite(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

	vector<PCKINDEXTABLE_COMPRESS> cPckIndexTable(dwValidFileCount);

	//��ʹ��Enum���б�����������_PCK_INDEX_TABLE

	LPPCKINDEXTABLE lpPckIndexTableSource = pckAllInfo.lpPckIndexTable;
	pckAllInfo.dwFileCountToAdd = 0;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(CheckIfNeedForcedStopWorking()) {
			Logger.w(TEXT_USERCANCLE);
			break;
		}

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		LPBYTE lpBufferToRead;

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;
		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//����ԭ���ĵ�ַ

		if (0 != dwNumberOfBytesToMap) {

			if (NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
				Logger_el(TEXT_VIEWMAP_FAIL);
				return FALSE;
			}

			cFileWrite.Write2(dwAddress, lpBufferToRead, dwNumberOfBytesToMap);
			cFileRead.UnmapViewAll();

		}

		//д����ļ���PckFileIndex�ļ�ѹ����Ϣ��ѹ��
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//���ļ���ѹ��������ʼ��ַ

		dwAddress += dwNumberOfBytesToMap;	//��һ���ļ���ѹ��������ʼ��ַ

		FillAndCompressIndexData(&cPckIndexTable[pckAllInfo.dwFileCountToAdd], &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//��ԭԭ���ĵ�ַ

		++lpPckIndexTableSource;
		++(pckAllInfo.dwFileCountToAdd);
		SetParams_ProgressInc();

	}

	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;
	pckAllInfo.lpPckIndexTableToAdd = &cPckIndexTable;

	//�رն��ļ�
	//д�ļ�����
	pckAllInfo.dwAddressOfFileEntry = dwAddress;

	WriteAllIndex(&cFileWrite, &pckAllInfo, dwAddress);

	WriteHeadAndTail(&cFileWrite, &pckAllInfo, dwAddress);

	//�̱߳��
	SetThreadFlag(FALSE);

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;
}


//��ѹ���ļ�
BOOL CPckClassWriteOperator::RecompressPckFile(const wchar_t * szRecompressPckFile, int iStripMode)
{

	Logger.i(TEXT_LOG_RECOMPRESS);

	//QWORD	dwAddress = PCK_DATA_START_AT;
	//DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	THREAD_PARAMS		cThreadParams;
	CMapViewFileMultiPckRead	cFileRead;

#pragma region ����ѹ������
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
#pragma endregion

	//����ͷ��βʱ��Ҫ�Ĳ���
	PCK_ALL_INFOS		pckAllInfo;
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	wcscpy_s(pckAllInfo.szNewFilename, szRecompressPckFile);

	//���ý����������ֵ
	SetParams_ProgressUpper(dwNoDupFileCount);

	//��Դ�ļ� 
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename))
		return FALSE;

#pragma region ����Ŀ���ļ�
	CMapViewFileMultiPckWrite cFileWriter(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(pckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

#pragma endregion

	cThreadParams.cDataFetchMethod.lpFileReadPCK = &cFileRead;
	cThreadParams.cDataFetchMethod.iStripFlag = iStripMode;
	cThreadParams.cDataFetchMethod.dwProcessIndex = 0;
	cThreadParams.cDataFetchMethod.dwTotalIndexCount = pckAllInfo.dwFileCount;
	cThreadParams.cDataFetchMethod.lpPckIndexTablePtrSrc = pckAllInfo.lpPckIndexTable;

	cThreadParams.lpPckClassThreadWorker = this;
	cThreadParams.lpFileWrite = &cFileWriter;
	cThreadParams.pck_data_src = DATA_FROM_PCK;
	cThreadParams.dwAddressStartAt = PCK_DATA_START_AT;
	cThreadParams.lpPckAllInfo = &pckAllInfo;
	cThreadParams.pckParams = m_lpPckParams;
	cThreadParams.dwFileCountOfWriteTarget = dwNoDupFileCount;

	//д�ļ�����
	pckAllInfo.lpPckIndexTable = NULL;
	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;

	CPckThreadRunner m_threadRunner(&cThreadParams);
	m_threadRunner.start();

	Logger.n(TEXT_LOG_WORKING_DONE);

	return TRUE;
}