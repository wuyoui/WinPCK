//////////////////////////////////////////////////////////////////////
// PckClass.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �й���ĳ�ʼ����
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4146 )
#pragma warning ( disable : 4267 )

CPckClass::CPckClass(LPPCK_RUNTIME_PARAMS inout)
{
	m_lpPckParams = inout;
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
}

CPckClass::~CPckClass()
{
	Logger.OutputVsIde(__FUNCTION__"\r\n");
}

BOOL CPckClass::Init(LPCWSTR	szFile)
{
	wcscpy_s(m_PckAllInfo.szFilename, szFile);
	GetFileTitleW(m_PckAllInfo.szFilename, m_PckAllInfo.szFileTitle, MAX_PATH);

	if(!MountPckFile(m_PckAllInfo.szFilename)) {

		ResetPckInfos();

		return FALSE;
	} else {

		BuildDirTree();
		return (m_PckAllInfo.isPckFileLoaded = TRUE);
	}
}
