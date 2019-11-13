//////////////////////////////////////////////////////////////////////
// ZupClass.cpp: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"

CZupClass::CZupClass(LPPCK_RUNTIME_PARAMS inout) : 
	CPckClass(inout),
	m_lpZupIndexTable(NULL)
{}

CZupClass::~CZupClass()
{
	if(NULL != m_lpZupIndexTable)
		free(m_lpZupIndexTable);
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

CONST	LPPCKINDEXTABLE CZupClass::GetPckIndexTable()
{
	return m_lpZupIndexTable;
}

void CZupClass::BuildDirTree()
{
	CMapViewFileMultiPckRead	cReadfile;

	if(!cReadfile.OpenPckAndMappingRead(m_PckAllInfo.szFilename))
		return;

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;
	LPPCKINDEXTABLE lpZupIndexTable = m_lpZupIndexTable;

	for(uint32_t i = 0;i < m_PckAllInfo.dwFileCount;i++) {

		//��element\��ͷ�Ķ���Ҫ����
		//����ֱ�Ӹ���
		//"element\" = 0x6d656c65, 0x5c746e656d656c65
		if(0x6d656c65 == *(uint32_t*)lpPckIndexTable->cFileIndex.szFilename) {

			//�����ļ���
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeof(PCKINDEXTABLE));
			DecodeFilename(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, lpPckIndexTable->cFileIndex.szFilename);

			uint8_t	*lpbuffer = cReadfile.View(lpZupIndexTable->cFileIndex.dwAddressOffset, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
			if(NULL == lpbuffer) {

				Logger_el(TEXT_VIEWMAPNAME_FAIL, m_PckAllInfo.szFilename);
				return;
			}

			if(lpZupIndexTable->cFileIndex.dwFileCipherTextSize == lpZupIndexTable->cFileIndex.dwFileClearTextSize) {

				lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(uint32_t*)lpbuffer;

				lpZupIndexTable->cFileIndex.dwAddressOffset += 4;
				lpZupIndexTable->cFileIndex.dwFileCipherTextSize -= 4;

			} else {

				if(m_zlib.check_zlib_header(lpbuffer)) {
					ulong_t	dwFileBytesRead = 4;
					m_zlib.decompress_part((uint8_t*)&lpZupIndexTable->cFileIndex.dwFileClearTextSize, &dwFileBytesRead,
						lpbuffer, lpZupIndexTable->cFileIndex.dwFileCipherTextSize, lpZupIndexTable->cFileIndex.dwFileCipherTextSize);
				} else {
					lpZupIndexTable->cFileIndex.dwFileClearTextSize = *(uint32_t*)lpbuffer;
				}

				lpZupIndexTable->cFileIndex.dwFileCipherTextSize = lpPckIndexTable->cFileIndex.dwFileClearTextSize;
			}

			cReadfile.UnmapViewAll();
		} else {
			//ֱ�Ӹ���
			memcpy(lpZupIndexTable, lpPckIndexTable, sizeof(PCKINDEXTABLE));
			CPckClassCodepage::PckFilenameCode2UCS(lpZupIndexTable->cFileIndex.szFilename, lpZupIndexTable->cFileIndex.szwFilename, sizeof(lpZupIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));
		}

		lpPckIndexTable++;
		lpZupIndexTable++;
	}

	//����Ŀ¼
	ParseIndexTableToNode(m_lpZupIndexTable);

}

BOOL CZupClass::Init(LPCWSTR szFile)
{
	wcscpy_s(m_PckAllInfo.szFilename, szFile);
	GetFileTitleW(m_PckAllInfo.szFilename, m_PckAllInfo.szFileTitle, MAX_PATH);

	if(MountPckFile(m_PckAllInfo.szFilename)) {

		if(NULL == (m_lpZupIndexTable = AllocPckIndexTableByFileCount())) {
			return FALSE;
		}

		//��һ��ѭ������ȡinc�ļ��������ֵ�
		m_lpDictHash = new CDictHash();
		if(BuildZupBaseDict()) {

			//�ڶ���ѭ���������ļ���������Ŀ¼��
			BuildDirTree();
		}

		//ɾ���ֵ�
		delete m_lpDictHash;
		return (m_PckAllInfo.isPckFileLoaded = TRUE);
	} else {
		ResetPckInfos();
		return FALSE;
	}
}

