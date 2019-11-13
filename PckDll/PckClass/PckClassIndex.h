#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassCodepage.h"
#include "PckClassZlib.h"
#include "MapViewFileMultiPck.h"

class CPckClassIndex :
	public virtual CPckClassBaseFeatures
{
public:
	CPckClassIndex();
	~CPckClassIndex();

	//�����ļ����������ļ��������ڴ�
	LPPCKINDEXTABLE		AllocPckIndexTableByFileCount();
	//��lpPckIndexTable->cFileIndex.szFilename �е�Ansi�ַ�ת��־Unicode��д��lpPckIndexTable->cFileIndex.szwFilename��
	void		GenerateUnicodeStringToIndex();
	// �ļ�ͷ��β�Ƚṹ��Ķ�ȡ
	BOOL		ReadPckFileIndexes();

protected:

	void*	AllocMemory(size_t	sizeStuct);
	//�ؽ�ʱ���¼����ļ���������ȥ��Ч�ĺ��ļ����ظ���
	DWORD	ReCountFiles();
	//���޸ĺ���������ݰ��汾���뵽�ṹ���в�ѹ����
	LPPCKINDEXTABLE_COMPRESS FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress);

};

