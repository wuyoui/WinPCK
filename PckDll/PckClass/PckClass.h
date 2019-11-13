//////////////////////////////////////////////////////////////////////
// PckClass.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "pck_default_vars.h"

#include "PckClassVersionDetect.h"
#include "PckClassWriteOperator.h"

#if !defined(_PCKCLASS_H_)
#define _PCKCLASS_H_

class CPckClass : 
	public virtual CPckClassWriteOperator,
	public virtual CPckClassVersionDetect
{
//����
public:
	CPckClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CPckClass();

	virtual BOOL	Init(const wchar_t * szFile);

#pragma region PckClassExtract.cpp

	BOOL	ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, const wchar_t *lpszDestDirectory);
	BOOL	ExtractAllFiles(const wchar_t *lpszDestDirectory);

private:
	//��ѹ�ļ�
	BOOL	ExtractFiles(const PCKINDEXTABLE **lpIndexToExtract, int nFileCount);
	BOOL	ExtractFiles(const PCK_PATH_NODE **lpNodeToExtract, int nFileCount);

public:
	//Ԥ���ļ�
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);
private:
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, const PCKINDEXTABLE* const lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);
	//PckClassExtract.cpp
	BOOL	StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpMapAddress);
	BOOL	DecompressFile(const wchar_t * lpszFilename, const PCKINDEXTABLE* lpPckFileIndexTable, LPVOID lpvoidFileRead);
#pragma endregion

#pragma region PckClassMount.cpp
protected:
	//PckClass.cpp
	BOOL	MountPckFile(const wchar_t * szFile);
private:
	void	BuildDirTree();
#pragma endregion




};

#endif