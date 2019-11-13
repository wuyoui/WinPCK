//////////////////////////////////////////////////////////////////////
// ZupClassExtract.cpp: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"

#pragma warning ( disable : 4267 )

BOOL CZupClass::GetSingleFileData(const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer)
{

	const PCKINDEXTABLE* lpPckFileIndexTable;

	//"element\" = 0x6d656c65, 0x5c746e656d656c65
	if(0x6d656c65 == *(uint32_t*)lpZupFileIndexTable->cFileIndex.szFilename) {

		lpPckFileIndexTable = GetBaseFileIndex(lpZupFileIndexTable, m_lpZupIndexTable);

		ulong_t	dwFileLengthDecompress2 = lpZupFileIndexTable->cFileIndex.dwFileClearTextSize;
		ulong_t	dwFileLengthDecompress1 = lpPckFileIndexTable->cFileIndex.dwFileClearTextSize;

		if(0 != sizeOfBuffer && sizeOfBuffer < dwFileLengthDecompress2) {
			dwFileLengthDecompress2 = sizeOfBuffer;
			if(sizeOfBuffer < dwFileLengthDecompress1)
				dwFileLengthDecompress1 = sizeOfBuffer;
		}

		char	*_cipherbuf = (char*)malloc(dwFileLengthDecompress1);

		if(NULL == _cipherbuf) {
			Logger_el(TEXT_MALLOC_FAIL);
			return FALSE;
		}

		if(CPckClass::GetSingleFileData(lpPckFileIndexTable, _cipherbuf, dwFileLengthDecompress1)) {

			if(m_zlib.check_zlib_header(_cipherbuf + 4)) {

				if(Z_OK != m_zlib.decompress_part((uint8_t*)buffer, &dwFileLengthDecompress2,
					(uint8_t*)_cipherbuf + 4, dwFileLengthDecompress1 - 4, lpZupFileIndexTable->cFileIndex.dwFileClearTextSize)) {
					if(lpZupFileIndexTable->cFileIndex.dwFileClearTextSize == lpZupFileIndexTable->cFileIndex.dwFileCipherTextSize) {
						memcpy(buffer, _cipherbuf + 4, dwFileLengthDecompress2);
					} else {
						assert(FALSE);
						Logger_el(TEXT_UNCOMPRESSDATA_FAIL, lpZupFileIndexTable->cFileIndex.szFilename);
					}
				}
			} else {
				memcpy(buffer, _cipherbuf + 4, dwFileLengthDecompress2);
			}

			free(_cipherbuf);

		} else {
			free(_cipherbuf);
			return FALSE;
		}


	} else {

		lpPckFileIndexTable = lpZupFileIndexTable;
		return CPckClass::GetSingleFileData(lpPckFileIndexTable, buffer, sizeOfBuffer);
	}

	return TRUE;

}