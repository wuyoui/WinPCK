#include "PckClassWriteOperator.h"

//�������ļ�
BOOL CPckClassWriteOperator::RenameFilename()
{
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
	Logger.i(TEXT_LOG_RENAME);

	//�����Ǵ���һ���ļ�����������ѹ������ļ�
	CMapViewFileMultiPckWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		Logger_el(UCSTEXT(TEXT_OPENWRITENAME_FAIL), m_PckAllInfo.szFilename);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());

	if(!cFileWrite.Mapping(dwFileSize)) {

		Logger_el(UCSTEXT(TEXT_CREATEMAPNAME_FAIL), m_PckAllInfo.szFilename);
		assert(FALSE);
		return FALSE;
	}

	//д�ļ�����
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFileEntry;

	WriteAllIndex(&cFileWrite, &m_PckAllInfo, dwAddress);
	
	WriteHeadAndTail(&cFileWrite, &m_PckAllInfo, dwAddress, FALSE);

	Logger.i(TEXT_LOG_WORKING_DONE);

	return TRUE;
}