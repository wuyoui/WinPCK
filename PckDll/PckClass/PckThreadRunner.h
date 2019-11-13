#pragma once

#include "MapViewFileMultiPck.h"
#include "PckClassWriteOperator.h"
#include "PckClassLog.h"

#include <functional>
#include <deque>
#include <mutex>


//��ȡ�ļ���ʽ
typedef enum { DATA_FROM_FILE, DATA_FROM_PCK } PCK_DATA_SOURCE;

//��ȡ�ļ�����ֵ
typedef enum { FD_OK, FD_END, FD_ERR, FD_CANCEL } FETCHDATA_RET;

typedef std::function<FETCHDATA_RET(PCKINDEXTABLE&)> FETCHDATA_FUNC;

class CPckClassWriteOperator;

typedef struct _DataFetchMethod
{
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesList;
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesListEnd;

	CMapViewFileMultiPckRead		*lpFileReadPCK;
	LPPCKINDEXTABLE					lpPckIndexTablePtrSrc;
	uint32_t						dwProcessIndex;
	uint32_t						dwTotalIndexCount;
	int								iStripFlag;
	//�������߳��е�ǰ��ʾ�ģ��ڵ��Ӧ��pck�е��ļ�·��
	wchar_t							szCurrentNodeString[MAX_PATH_PCK_260];
	//�䳤��
	int								nCurrentNodeStringLen;

}DATA_FETCH_METHOD, *LPDATA_FETCH_METHOD;

typedef struct _ThreadParams
{
	PCK_DATA_SOURCE				pck_data_src;
	DATA_FETCH_METHOD			cDataFetchMethod;
	CMapViewFileMultiPckWrite *	lpFileWrite;
	CPckClassWriteOperator *	lpPckClassThreadWorker;
	LPPCK_ALL_INFOS				lpPckAllInfo;
	LPPCK_RUNTIME_PARAMS		pckParams;

	uint64_t					dwAddressStartAt;

	//int							threadnum;
	//Ԥ�Ƶ�ѹ���ļ���С
	uint64_t					qwCompressTotalFileSize;
	//д�����ݵ�Ŀ������,һ��=mt_dwFileCount�����ʱ=��ѹ��ʱ����Ч�ļ�����
	uint32_t					dwFileCountOfWriteTarget;
}THREAD_PARAMS, *LPTHREAD_PARAMS;

#define MALLOCED_EMPTY_DATA			(1)

template <typename T>
_inline T * __fastcall mystrcpy(T * dest, const T *src)
{
	while ((*dest = *src))
		dest++, src++;
	return dest;
}


typedef std::function<void()> CompressThreadFunc;
typedef std::function<void()> WriteThreadFunc;

class CPckThreadRunner
{
public:
	CPckThreadRunner(LPTHREAD_PARAMS threadparams);
	virtual ~CPckThreadRunner();
	CPckThreadRunner(CPckThreadRunner const&) = delete;
	CPckThreadRunner& operator=(CPckThreadRunner const&) = delete;

	void start();


private:
	LPTHREAD_PARAMS			m_threadparams = nullptr;
	LPPCK_RUNTIME_PARAMS	m_lpPckParams = nullptr;
	CPckClassWriteOperator * m_lpPckClassBase;

	std::mutex				m_LockCompressedflag;

	std::mutex				m_LockReadFileMap;
#if PCK_DEBUG_OUTPUT
	std::mutex				m_LockThreadID;
	int						m_threadID = 0;		//�߳�ID
#endif

private:
#if PCK_DEBUG_OUTPUT
	uint32_t				m_dwCurrentQueuePosPut = 0, m_dwCurrentQueuePosGet = 0;				//��ǰ����λ��
#endif

protected:
	uint64_t				mt_dwAddressQueue;												//ȫ��ѹ�����̵�д�ļ���λ�ã�ֻ��Queue����
	uint64_t				mt_dwAddressNameQueue;											//������pck�ļ���ѹ���ļ���������ʼλ��

private:
	std::mutex					m_LockQueue, m_LockMaxMemory;

	std::condition_variable		m_cvReadyToPut, m_cvMemoryNotEnough;
	BOOL						m_memoryNotEnoughBlocked = FALSE;

	deque<PCKINDEXTABLE>		m_QueueContent;
	vector<PCKINDEXTABLE_COMPRESS> m_Index_Compress;



private:

	void startThread();

	void CompressThread(FETCHDATA_FUNC GetUncompressedData);
	void WriteThread(LPTHREAD_PARAMS threadparams);

	//ѹ��ʱ�ڴ�ʹ��
	FETCHDATA_RET	detectMaxToAddMemory(DWORD dwMallocSize);
	FETCHDATA_RET	detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	freeMaxToSubtractMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);

	//ѹ�����ݶ���

	BOOL	putCompressedDataQueue(PCKINDEXTABLE &lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable);

	//�ڶ��߳������л�ȡѹ���õ�Դ����
	FETCHDATA_RET		GetUncompressedDataFromFile(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);
	FETCHDATA_RET		GetUncompressedDataFromPCK(LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);

};

