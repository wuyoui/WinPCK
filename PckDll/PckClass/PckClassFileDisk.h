#pragma once

typedef unsigned __int64	QWORD;
#include "PckStructs.h"
#include "PckClassLog.h"
#include "MapViewFileMultiPck.h"
#include "PckClassBaseFeatures.h"


class CPckClassFileDisk :
	protected virtual CPckClassBaseFeatures
{
public:
	CPckClassFileDisk();
	~CPckClassFileDisk();

protected:
	BOOL		MakeFolderExist(const wchar_t* lpszDirectory);
private:
	BOOL		MakeFolderExistInternal(const wchar_t* lpszDirectory);
protected:
	BOOL		EnumAllFilesByPathList(const vector<wstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList);

	//�ؽ�pckʱ��Ҫ�Ĵ��̿ռ䣨Ŀ���ļ��������ؽ���PCK�ļ���С��
	QWORD	GetPckFilesizeRebuild(const wchar_t * lpszFilename, QWORD qwPckFilesize);

	//������ʱ��Ҫ���ļ��Ĵ�С
	QWORD	GetPckFilesizeRename(const wchar_t * lpszFilename, QWORD qwCurrentPckFilesize);

	//��Ӻ��½��ļ�ʱ��д���pck�ļ���С
	QWORD	GetPckFilesizeByCompressed(const char* lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	QWORD	GetPckFilesizeByCompressed(const wchar_t * lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);

private:

	//qwCurrentPckFilesizeΪ�Ѿ����ڵ��ļ���С��qwToAddSpace����Ҫ����Ĵ�С������ֵΪ��qwCurrentPckFilesize + ���������������С��
	QWORD	GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);
	//�����ļ���
	VOID	EnumFile(LPWSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen);

};

