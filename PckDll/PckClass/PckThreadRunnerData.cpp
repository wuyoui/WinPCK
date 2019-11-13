#include "PckThreadRunner.h"

#include "PckModelStrip.h"

//�ڶ��߳������л�ȡδѹ���õ�Դ����
FETCHDATA_RET CPckThreadRunner::GetUncompressedDataFromFile(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while (1) {

		std::unique_lock<std::mutex> lckCompressedflag(m_LockCompressedflag);

		if (lpDataFetchMethod->ciFilesList == lpDataFetchMethod->ciFilesListEnd)
			return FD_END;

		vector<FILES_TO_COMPRESS>::const_pointer lpOneFile = &lpDataFetchMethod->ciFilesList[0];
		lpDataFetchMethod->ciFilesList++;
		lckCompressedflag.unlock();

#if PCK_DEBUG_OUTPUT
		Logger.logOutput(__FUNCTION__, "lpfirstFile_id=%d\r\n", lpOneFile->id);
#endif

		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = m_lpPckClassBase->m_zlib.GetCompressBoundSizeByFileSize(pckFileIndex.cFileIndex.dwFileClearTextSize, pckFileIndex.cFileIndex.dwFileCipherTextSize, lpOneFile->dwFileSize);

		//�����ļ���
		memcpy(mystrcpy(pckFileIndex.cFileIndex.szwFilename, lpDataFetchMethod->szCurrentNodeString), lpOneFile->szwFilename + lpOneFile->nFileTitleLen, lpOneFile->nBytesToCopy - lpDataFetchMethod->nCurrentNodeStringLen);
		//Unicode�ļ���ת��ΪCP936��ANSI
		CPckClassCodepage::PckFilenameCode2Ansi(pckFileIndex.cFileIndex.szwFilename, pckFileIndex.cFileIndex.szFilename, sizeof(pckFileIndex.cFileIndex.szwFilename));

		//����ļ���СΪ0�����������ļ�����
		if (0 != pckFileIndex.cFileIndex.dwFileClearTextSize) {
			CMapViewFileRead		cFileRead;
			LPBYTE					lpBufferToRead;
			//�ļ���Ϊ0ʱ�Ĵ���
			//��Ҫ����ѹ�����ļ�
			if (NULL == (lpBufferToRead = cFileRead.OpenMappingViewAllRead(lpOneFile->szwFilename))) {
				m_lpPckClassBase->SetErrMsgFlag(PCK_ERR_OPENMAPVIEWR);
				return FD_ERR;
			}

			//�ж�ʹ�õ��ڴ��Ƿ񳬹����ֵ
			FETCHDATA_RET rtn;
			if (FD_OK != (rtn = detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			if (PCK_BEGINCOMPRESS_SIZE < pckFileIndex.cFileIndex.dwFileClearTextSize) {
				m_lpPckClassBase->m_zlib.compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize,
					lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			}
			else {
				memcpy(lpCompressedBuffer, lpBufferToRead, pckFileIndex.cFileIndex.dwFileClearTextSize);
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;
	}
	return FD_END;
}

FETCHDATA_RET CPckThreadRunner::GetUncompressedDataFromPCK(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex)
{

	while (1) {

		std::unique_lock<std::mutex> lckCompressedflag(m_LockCompressedflag);
		if (lpDataFetchMethod->dwProcessIndex >= lpDataFetchMethod->dwTotalIndexCount) 
			return FD_END;

		DATA_FETCH_METHOD cDataFetchMethod;
		memcpy(&cDataFetchMethod, lpDataFetchMethod, sizeof(DATA_FETCH_METHOD));
		++(lpDataFetchMethod->lpPckIndexTablePtrSrc);
		++(lpDataFetchMethod->dwProcessIndex);
		lckCompressedflag.unlock();

		Logger.logOutput(__FUNCTION__, "dwProcessIndex=%d\r\n", cDataFetchMethod.dwProcessIndex);

		if (cDataFetchMethod.lpPckIndexTablePtrSrc->isInvalid)
			continue;

		LPBYTE				lpBufferToRead;
		//������ѹ�����ݵĽ�ѹ������
		LPBYTE				lpDecompressBuffer = NULL;
		//������ѹ�����ݵ�Դ����
		LPBYTE				lpSourceBuffer = NULL;

		LPPCKINDEXTABLE	lpPckIndexTablePtrSrc = cDataFetchMethod.lpPckIndexTablePtrSrc;
		ulong_t dwNumberOfBytesToMap = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
		ulong_t dwFileClearTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize;

		memcpy(&pckFileIndex.cFileIndex, &lpPckIndexTablePtrSrc->cFileIndex, sizeof(PCKFILEINDEX));

		if (PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = m_lpPckClassBase->m_zlib.compressBound(dwFileClearTextSize);
		}
		else {
			pckFileIndex.cFileIndex.dwFileCipherTextSize = dwFileClearTextSize;
		}

		LPBYTE lpCompressedBuffer = (BYTE*)MALLOCED_EMPTY_DATA;
		pckFileIndex.dwMallocSize = pckFileIndex.cFileIndex.dwFileCipherTextSize;

		if (0 != dwFileClearTextSize) {

			//�ж�ʹ�õ��ڴ��Ƿ񳬹����ֵ
			FETCHDATA_RET rtn;
			if (FD_OK != (rtn = detectMaxAndAddMemory(lpCompressedBuffer, pckFileIndex.dwMallocSize))) {
				return rtn;
			}

			//�ļ�������Ҫ��ѹ��
			if (PCK_BEGINCOMPRESS_SIZE < dwFileClearTextSize) {
				//����Դ���ݵĿռ�
				if (FD_OK != (rtn = detectMaxAndAddMemory(lpSourceBuffer, dwNumberOfBytesToMap))) {
					return rtn;
				}

				if (FD_OK != (rtn = detectMaxAndAddMemory(lpDecompressBuffer, dwFileClearTextSize))) {
					return rtn;
				}

				std::unique_lock<std::mutex> lckCReadFileMap(m_LockReadFileMap);
				if (NULL == (lpBufferToRead = cDataFetchMethod.lpFileReadPCK->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap))) {
					lckCReadFileMap.unlock();
					freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
					freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);

					return FD_ERR;
					break;
				}

				memcpy(lpSourceBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				cDataFetchMethod.lpFileReadPCK->UnmapViewAll();
				lckCReadFileMap.unlock();

				if (m_lpPckClassBase->m_zlib.check_zlib_header(lpSourceBuffer)) {

					m_lpPckClassBase->m_zlib.decompress(lpDecompressBuffer, &dwFileClearTextSize, lpSourceBuffer, dwNumberOfBytesToMap);

					if (dwFileClearTextSize == lpPckIndexTablePtrSrc->cFileIndex.dwFileClearTextSize) {

						/*
						��������뾫�����
						*/
						if (PCK_STRIP_NONE != cDataFetchMethod.iStripFlag) {
							CPckModelStrip cModelStrip;
							cModelStrip.StripContent(lpDecompressBuffer, &pckFileIndex.cFileIndex, cDataFetchMethod.iStripFlag);
						}

						m_lpPckClassBase->m_zlib.compress(lpCompressedBuffer, &pckFileIndex.cFileIndex.dwFileCipherTextSize, lpDecompressBuffer, pckFileIndex.cFileIndex.dwFileClearTextSize);
					}
					else {
						memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
						pckFileIndex.cFileIndex.dwFileCipherTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
					}

				}
				else {
					memcpy(lpCompressedBuffer, lpSourceBuffer, dwNumberOfBytesToMap);
					pckFileIndex.cFileIndex.dwFileCipherTextSize = lpPckIndexTablePtrSrc->cFileIndex.dwFileCipherTextSize;
				}

				freeMaxAndSubtractMemory(lpSourceBuffer, dwNumberOfBytesToMap);
				freeMaxAndSubtractMemory(lpDecompressBuffer, dwFileClearTextSize);


			}
			else {
#pragma region �ļ���С����Ҫѹ��ʱ
				std::lock_guard<std::mutex> lckCReadFileMap(m_LockReadFileMap);

				if (NULL == (lpBufferToRead = cDataFetchMethod.lpFileReadPCK->View(lpPckIndexTablePtrSrc->cFileIndex.dwAddressOffset, dwNumberOfBytesToMap)))
					return FD_ERR;

				memcpy(lpCompressedBuffer, lpBufferToRead, dwNumberOfBytesToMap);
				cDataFetchMethod.lpFileReadPCK->UnmapViewAll();
#pragma endregion
			}

		}

		pckFileIndex.compressed_file_data = lpCompressedBuffer;
		return FD_OK;
	}
	return FD_END;
}