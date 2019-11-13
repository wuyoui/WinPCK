
#include "PckClassWriteOperator.h"

//�½�������pck��
BOOL CPckClassWriteOperator::UpdatePckFile(const wchar_t * szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0;			//�ļ�����, ԭpck�ļ��е��ļ���
	DWORD		dwDuplicateFileCount = 0;
	uint64_t		qwTotalNewFileSize = 0;						//δѹ��ʱ�����ļ���С

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	uint64_t		dwAddressWhereToAppendData;
	THREAD_PARAMS		cThreadParams;

	//��ʼ�����ļ�
	const PCK_PATH_NODE*		lpNodeToInsertPtr;

#pragma region ����ѹ������
	m_zlib.init_compressor(level);
#pragma endregion


#pragma region ���ò���
	
	m_FilesToBeAdded.clear();
	m_PckAllInfo.lpFilesToBeAdded = &m_FilesToBeAdded;

	if(m_PckAllInfo.isPckFileLoaded) {

		lpNodeToInsertPtr = lpNodeToInsert;

		//���ļ�β������ݣ�����ʧ�ܺ�ɻ���
		dwAddressWhereToAppendData = m_PckAllInfo.qwPckSize;

		//ȡ�õ�ǰ�ڵ�����·��
		if(!GetCurrentNodeString(cThreadParams.cDataFetchMethod.szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = wcslen(cThreadParams.cDataFetchMethod.szCurrentNodeString);

		Logger.i(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {
		//�½��ļ�
		//m_PckAllInfo.dwAddressOfFileEntry = PCK_DATA_START_AT;
		dwAddressWhereToAppendData = PCK_DATA_START_AT;

		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*cThreadParams.cDataFetchMethod.szCurrentNodeString = 0;
		cThreadParams.cDataFetchMethod.nCurrentNodeStringLen = 0;

		Logger.i(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	wcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);

#pragma endregion

#pragma region ��������ӵ��ļ� 
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, m_PckAllInfo.lpFilesToBeAdded))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//����˵����
	// mt_dwFileCount	��ӵ��ļ����������ظ�
	// dwFileCount		�������ʹ�ò�����������ļ�������н�ʹ�ô˲�����ʾ��ӵ��ļ������������ظ�

	//�ļ���д�봰�����б�������ʾ����
	SetParams_ProgressUpper(dwNewFileCount);

	//��������Ҫ���ռ�qwTotalFileSize
	cThreadParams.qwCompressTotalFileSize = GetPckFilesizeByCompressed(szPckFile, qwTotalNewFileSize, m_PckAllInfo.qwPckSize);

	//��ԭ��Ŀ¼�е��ļ��Աȣ��Ƿ�������
	//���ԣ����������ǰ�				��������Ҷ�Ϊ�ļ����ļ��У��򸲸�
	//
	//����FindFileNode����-1�˳�������0����ʾֱ����ӣ���0�������ظ���
	//дר�õ�writethread��compressthread,�Ե���
	//��PCKINDEXTABLE_COMPRESS�����addר�����ԣ����ж��Ƿ����ô˽ڵ㣨����ʱ��0ʹ�ã�1��ʹ��
	//���� ʱʹ��2��ѭ��д��ѹ������ 

	//dwFileCount�����ڴ˴�ָ������ӵ��ļ���ȥ���������� 
	if(m_PckAllInfo.isPckFileLoaded) {
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwDuplicateFileCount))
			return FALSE;
	}

	//��־
	Logger.i(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, cThreadParams.qwCompressTotalFileSize);

#pragma region ����Ŀ���ļ�
	CMapViewFileMultiPckWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, cThreadParams.qwCompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	cThreadParams.cDataFetchMethod.ciFilesList = m_PckAllInfo.lpFilesToBeAdded->cbegin();
	cThreadParams.cDataFetchMethod.ciFilesListEnd = m_PckAllInfo.lpFilesToBeAdded->cend();

	cThreadParams.lpFileWrite = &cFileWriter;
	cThreadParams.pck_data_src = DATA_FROM_FILE;
	cThreadParams.dwFileCountOfWriteTarget = dwNewFileCount;
	cThreadParams.lpPckClassThreadWorker = this;
	cThreadParams.dwAddressStartAt = dwAddressWhereToAppendData;
	cThreadParams.lpPckAllInfo = &m_PckAllInfo;
	cThreadParams.pckParams = m_lpPckParams;

	//д�ļ�����
	m_PckAllInfo.dwFileCount = m_PckAllInfo.dwFileCountOld - dwDuplicateFileCount;

	CPckThreadRunner m_threadRunner(&cThreadParams);
	m_threadRunner.start();

	//���������´�һ�Σ�����ֱ�Ӵ򿪣��ɽ����߳����
	m_lpPckParams->cVarParams.dwOldFileCount = m_PckAllInfo.dwFileCountOld;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = dwNewFileCount;
	m_lpPckParams->cVarParams.dwChangedFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = dwDuplicateFileCount;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFinalFileCount;

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
