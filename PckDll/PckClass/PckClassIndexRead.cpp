#include "PckClassIndex.h"
#include "PckClassZlib.h"


BOOL CPckClassIndex::ReadPckFileIndexes()
{
	CMapViewFileMultiPckRead cRead;

	if(!cRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		Logger_el(UCSTEXT(TEXT_OPENNAME_FAIL), m_PckAllInfo.szFilename);
		return FALSE;
	}

	if(!(NULL != (m_PckAllInfo.lpPckIndexTable = AllocPckIndexTableByFileCount()))) {
		return FALSE;
	}

	//��ʼ���ļ�
	BYTE	*lpFileBuffer;
	if(NULL == (lpFileBuffer = cRead.View(m_PckAllInfo.dwAddressOfFileEntry, cRead.GetFileSize() - m_PckAllInfo.dwAddressOfFileEntry))) {
		Logger_el(TEXT_VIEWMAP_FAIL);
		return FALSE;
	}

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;
	BOOL			isLevel0;
	DWORD			byteLevelKey;
	//���ÿ���ļ�������ͷ��������DWORDѹ�����ݳ�����Ϣ
	DWORD			dwFileIndexTableCryptedDataLength[2];
	DWORD			dwFileIndexTableClearDataLength = m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize;
	DWORD			IndexCompressedFilenameDataLengthCryptKey[2] = { \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey1, \
		m_PckAllInfo.lpDetectedPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey2 };

	//pck��ѹ��ʱ���ļ�����ѹ�����Ȳ��ᳬ��0x100�����Ե�
	//��ʼһ���ֽڣ����0x75����û��ѹ���������0x74����ѹ����	0x75->FILEINDEX_LEVEL0
	//cRead.SetFilePointer(m_PckAllInfo.dwAddressOfFileEntry, FILE_BEGIN);

	byteLevelKey = (*(DWORD*)lpFileBuffer) ^ IndexCompressedFilenameDataLengthCryptKey[0];
	isLevel0 = (m_PckAllInfo.lpDetectedPckVerFunc->dwFileIndexSize == byteLevelKey)/* ? TRUE : FALSE*/;

	if(isLevel0) {

		for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;i++) {
			//�ȸ�������ѹ�����ݳ�����Ϣ
			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);

			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;

			if(dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]) {

				Logger_el(TEXT_READ_INDEX_FAIL);
				return FALSE;
			}

			lpFileBuffer += 8;

			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, lpFileBuffer);
			lpFileBuffer += dwFileIndexTableClearDataLength;
			++lpPckIndexTable;

		}
	} else {

		for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {

			memcpy(dwFileIndexTableCryptedDataLength, lpFileBuffer, 8);
			*(QWORD*)dwFileIndexTableCryptedDataLength ^= *(QWORD*)IndexCompressedFilenameDataLengthCryptKey;
			lpFileBuffer += 8;

			if(dwFileIndexTableCryptedDataLength[0] != dwFileIndexTableCryptedDataLength[1]) {

				Logger_el(TEXT_READ_INDEX_FAIL);
				return FALSE;
			}
			
			DWORD dwFileBytesRead = MAX_INDEXTABLE_CLEARTEXT_LENGTH/*dwFileIndexTableClearDataLength*/;
			BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];

			m_zlib.decompress(pckFileIndexBuf, &dwFileBytesRead,
				lpFileBuffer, dwFileIndexTableCryptedDataLength[0]);

#if PCK_V2031_ENABLE
			/*
			������������С�ĳ���288���¼���4�ֽ�����
			PCKFILEINDEX_V2030->
			*/
			PCKFILEINDEX_V2031* testnewindex = (PCKFILEINDEX_V2031*)pckFileIndexBuf;
#endif

			m_PckAllInfo.lpDetectedPckVerFunc->PickIndexData(&lpPckIndexTable->cFileIndex, pckFileIndexBuf);

			lpFileBuffer += dwFileIndexTableCryptedDataLength[0];
			++lpPckIndexTable;

		}
	}

	return TRUE;
}