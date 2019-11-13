//////////////////////////////////////////////////////////////////////
// CPckControlCenter.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma once

#include "PckStructs.h"
#include "PckClassLog.h"
#include <vector>

typedef struct _PCK_PATH_NODE * LPPCK_PATH_NODE;
typedef struct _PCK_RUNTIME_PARAMS * LPPCK_RUNTIME_PARAMS;

#if 0
#ifdef _WINDLL	//.dll
#define EXPORT_CLASS _declspec(dllexport) 
#elif defined(_DLL)	//.exe
#define EXPORT_CLASS _declspec(dllimport)
#else	//other
#define EXPORT_CLASS 
#endif
#else
#define EXPORT_CLASS 
#endif

class CPckClass;
class CPckClassLog;

class EXPORT_CLASS CPckControlCenter
{
	//����
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	void	New();

private:

	void	init();
	void	uninit();
	void	Reset(uint32_t dwUIProgressUpper = 1);
#pragma region �򿪹ر��ļ�

	void	Close();
public:

	BOOL	Open(const wchar_t * lpszFile);

#pragma endregion

#pragma region �������ڵ�

	//������һ���ڵ�
	BOOL	RenameEntry(LPENTRY lpFileEntry, LPCWSTR lpszReplaceString);
	//�ύ
	BOOL	RenameSubmit();

#pragma endregion

#pragma region Ԥ����ѹ�ļ�

	//Ԥ���ļ�
	BOOL		GetSingleFileData(LPCENTRY lpFileEntry, char *buffer, size_t sizeOfBuffer = 0);

	//��ѹ�ļ�
	BOOL		ExtractFiles(const PCK_UNIFIED_FILE_ENTRY **lpFileEntryArray, int nEntryCount, LPCWSTR lpszDestDirectory);
	BOOL		ExtractAllFiles(LPCWSTR lpszDestDirectory);

#pragma endregion

#pragma region �ؽ�pck�ļ�
	//�ؽ�pck�ļ�
	BOOL	TestScript(LPCWSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress);

#pragma endregion

#pragma region ��Ϸ����
	BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag);
#pragma endregion



#pragma region create or update pck file

	//����ļ��б����ѹ��ʱʹ�õ�vector����
	void	StringArrayReset();
	void	StringArrayAppend(LPCWSTR lpszFilePath);
	BOOL	UpdatePckFileSubmit(LPCWSTR szPckFile, LPCENTRY lpFileEntry);

#pragma endregion

#pragma region ɾ���ڵ�
	//ɾ��һ���ڵ�
	BOOL	DeleteEntry(LPCENTRY lpFileEntry);
	//�ύ
	BOOL	DeleteEntrySubmit();

#pragma endregion


#pragma region �汾���

	int		GetPckVersion();
	BOOL	SetPckVersion(int verID);
	//��ȡ��ǰ��������
	LPCWSTR	GetCurrentVersionName();
	static uint32_t	GetVersionCount();
	static LPCWSTR	GetVersionNameById(int verID);
	static int		AddVersionAlgorithmId(int AlgorithmId, int Version);

#pragma endregion

#pragma region �ڵ����Բ���

	LPCENTRY GetRootNode();
	//��ȡnode·��
	static BOOL			GetCurrentNodeString(LPWSTR szCurrentNodePathString, LPCENTRY lpFileEntry);
	LPCENTRY			GetFileEntryByPath(LPCWSTR _in_szCurrentNodePathString);
#pragma endregion

#pragma region pck�ļ�����

	BOOL				IsValidPck();

	//��ȡ�ļ���
	uint32_t			GetPckFileCount();
	//ʵ���ļ���С
	uint64_t			GetPckSize();
	uint64_t			GetPckDataAreaSize();
	uint64_t			GetPckRedundancyDataSize();

	static uint64_t		GetFileSizeInEntry(LPCENTRY lpFileEntry);
	static uint64_t		GetCompressedSizeInEntry(LPCENTRY lpFileEntry);
	static uint32_t		GetFoldersCountInEntry(LPCENTRY lpFileEntry);
	static uint32_t		GetFilesCountInEntry(LPCENTRY lpFileEntry);

	static size_t		GetFilelenBytesOfEntry(LPCENTRY lpFileEntry);
	static size_t		GetFilelenLeftBytesOfEntry(LPCENTRY lpFileEntry);

	static uint64_t		GetFileOffset(LPCENTRY lpFileEntry);

	//���ø�����Ϣ
	const char*			GetAdditionalInfo();
	static uint32_t		GetAdditionalInfoMaxSize();
	BOOL				SetAdditionalInfo(LPCSTR lpszAdditionalInfo);

	//�Ƿ���֧�ָ��µ��ļ�
	BOOL				isSupportAddFileToPck();

#pragma endregion

	//�򿪡��رա���ԭ���¼�ע��
	static void		regMsgFeedback(void* pTag, FeedbackCallback _FeedbackCallBack);
	static int		DefaultFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam);

#pragma region ��ѯ��Ŀ¼���
private:
	static void		DefaultShowFilelistCallback(void* _in_param, int sn, LPCWSTR lpszFilename, int entry_type, uint64_t qwFileSize, uint64_t qwFileSizeCompressed, void* fileEntry);

public:
	uint32_t		SearchByName(LPCWSTR lpszSearchString, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);
	static uint32_t	ListByNode(LPCENTRY lpFileEntry, void* _in_param, SHOW_LIST_CALLBACK _showListCallback);

#pragma endregion

#pragma region �߳̿���
	//�߳����в���
	BOOL			isThreadWorking();
	void			ForceBreakThreadWorking();
#pragma endregion

	//error no
	int				GetLastErrorMsg();
	BOOL			isLastOptSuccess();

#pragma region �ڴ�ռ��
	//�ڴ�ռ��
	uint32_t			getMTMemoryUsed();
	//�ڴ�ֵ
	void			setMTMaxMemory(uint32_t dwMTMaxMemory);
	uint32_t			getMTMaxMemory();

	//����ڴ�
	static uint32_t	getMaxMemoryAllowed();

#pragma endregion

#pragma region �߳���
	//�߳���
	uint32_t	getMaxThread();
	void	setMaxThread(uint32_t dwThread);
	//�߳�Ĭ�ϲ���
	static uint32_t	getMaxThreadUpperLimit();
#pragma endregion

#pragma region ѹ���ȼ�

	//ѹ���ȼ�
	uint32_t	getCompressLevel();
	void	setCompressLevel(uint32_t dwCompressLevel = Z_DEFAULT_COMPRESS_LEVEL);

	//ѹ���ȼ�Ĭ�ϲ���
	static uint32_t	getMaxCompressLevel();
	static uint32_t	getDefaultCompressLevel();
#pragma endregion

#pragma region �������

	uint32_t	getUIProgress();
	void	setUIProgress(uint32_t dwUIProgress);
	uint32_t	getUIProgressUpper();

#pragma endregion

#pragma region ��ӡ���/�����ļ����

	uint32_t	GetUpdateResult_OldFileCount();
	uint32_t	GetUpdateResult_PrepareToAddFileCount();
	uint32_t	GetUpdateResult_ChangedFileCount();
	uint32_t	GetUpdateResult_DuplicateFileCount();
	uint32_t	GetUpdateResult_FinalFileCount();

#pragma endregion

private:

	FMTPCK	GetPckTypeFromFilename(const wchar_t * lpszFile);

	LPPCK_PATH_NODE				m_lpPckRootNode;
	std::vector<std::wstring>	lpszFilePathToAdd;	//����Ӷ���ļ�ʱ�ṩ����

	std::wstring				szUpdateResultString;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;

	//��ʽ
	FMTPCK						m_emunFileFormat;

	static FeedbackCallback		pFeedbackCallBack;
	static void*				pTag;

};