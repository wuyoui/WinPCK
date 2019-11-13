
#include "PckClassIndex.h"
#include <thread>
#include <vector>

CPckClassIndex::CPckClassIndex()
{}

CPckClassIndex::~CPckClassIndex()
{
	if(NULL != m_PckAllInfo.lpPckIndexTable)
		free(m_PckAllInfo.lpPckIndexTable);
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

void* CPckClassIndex::AllocMemory(size_t	sizeStuct)
{
	void*		lpMallocNode;

	if(NULL == (lpMallocNode = malloc(sizeStuct))) {
		SetErrMsgFlag(PCK_ERR_MALLOC);
		return NULL;
	}
	//��ʼ���ڴ�
	memset(lpMallocNode, 0, sizeStuct);
	return lpMallocNode;
}

LPPCKINDEXTABLE	CPckClassIndex::AllocPckIndexTableByFileCount()
{
	//�����m_PckAllInfo.dwFileCount + 1�����һ��1������Ϊ���index�Ľ�������entryType��ΪPCK_ENTRY_TYPE_TAIL_INDEX
	//PCK_ENTRY_TYPE_TAIL_INDEX����index��������ݣ���Ϊ��ǽ�������
	return (LPPCKINDEXTABLE)AllocMemory(sizeof(PCKINDEXTABLE) * (m_PckAllInfo.dwFileCount + 1));
}

void CPckClassIndex::GenerateUnicodeStringToIndex()
{
	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//�ļ�������
		lpPckIndexTable->nFilelenBytes = strlen(lpPckIndexTable->cFileIndex.szFilename);
		//�ļ���ʣ��ռ�,��ռ������\0
		lpPckIndexTable->nFilelenLeftBytes = MAX_PATH_PCK_256 - lpPckIndexTable->nFilelenBytes - 1;
		//pck ansi -> unicode
		CPckClassCodepage::PckFilenameCode2UCS(lpPckIndexTable->cFileIndex.szFilename, lpPckIndexTable->cFileIndex.szwFilename, sizeof(lpPckIndexTable->cFileIndex.szwFilename) / sizeof(wchar_t));
		++lpPckIndexTable;
	}

}

//�ؽ�ʱ������Ч�ļ��������ų��ظ����ļ�
DWORD CPckClassIndex::ReCountFiles()
{
	DWORD deNewFileCount = 0;
	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		if(!lpPckIndexTableSource->isInvalid) {
			++deNewFileCount;
		}
		++lpPckIndexTableSource;
	}
	return deNewFileCount;
}

//���޸ĺ���������ݰ��汾���뵽�ṹ���в�ѹ����
LPPCKINDEXTABLE_COMPRESS CPckClassIndex::FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	lpPckIndexTableComped->dwIndexDataLength = MAX_INDEXTABLE_CLEARTEXT_LENGTH;

	m_zlib.compress(lpPckIndexTableComped->buffer, &lpPckIndexTableComped->dwIndexDataLength,
		m_PckAllInfo.lpSaveAsPckVerFunc->FillIndexData(lpPckFileIndexToCompress, pckFileIndexBuf), m_PckAllInfo.lpSaveAsPckVerFunc->dwFileIndexSize);
	//����ȡ��
	lpPckIndexTableComped->dwIndexValueHead = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey1;
	lpPckIndexTableComped->dwIndexValueTail = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.IndexCompressedFilenameDataLengthCryptKey2;

	return lpPckIndexTableComped;
}