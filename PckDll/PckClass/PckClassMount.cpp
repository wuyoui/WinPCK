//////////////////////////////////////////////////////////////////////
// PckClassMount.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �й���ĳ�ʼ����
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.27
//////////////////////////////////////////////////////////////////////
#include "PckClass.h"

BOOL CPckClass::MountPckFile(LPCWSTR	szFile)
{
	try {
		if (!DetectPckVerion(szFile))
			return FALSE;

		if (!ReadPckFileIndexes())
			return FALSE;
		//�����һ��Index��entryType��ΪPCK_ENTRY_TYPE_TAIL_INDEX
		m_PckAllInfo.lpPckIndexTable[m_PckAllInfo.dwFileCount].entryType = PCK_ENTRY_TYPE_TAIL_INDEX;
		return TRUE;
	}
	catch (MyException e) {
		Logger.e(e.what());
		return FALSE;
	}
	
}

void CPckClass::BuildDirTree()
{
	//����ȡ��index�е�ansi�ı�ȫ��ת��ΪUnicode
	GenerateUnicodeStringToIndex();
	//����index�е��ļ�������Ŀ¼��
	ParseIndexTableToNode(m_PckAllInfo.lpPckIndexTable);
}
