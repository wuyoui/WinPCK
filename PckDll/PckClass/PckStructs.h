//////////////////////////////////////////////////////////////////////
// PckStructs.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include "PckDefines.h"
//#include "pck_dependencies.h"
#include <vector>

#if !defined(_PCKSTRUCTS_H_)
#define _PCKSTRUCTS_H_


typedef struct _PCK_KEYS
{
	int32_t			id;
	wchar_t			name[64];
	PCK_CATEGORY	CategoryId;
	uint32_t		Version;
	//head
	uint32_t		HeadVerifyKey1;
	uint32_t		HeadVerifyKey2;
	//tail
	uint32_t		TailVerifyKey1;
	uint64_t		IndexesEntryAddressCryptKey;
	uint32_t		TailVerifyKey2;	//���ļ�β��-3��DWORD(xx xx xx xx 00 00 00 00 00 00 00 00)
	//index
	uint32_t		IndexCompressedFilenameDataLengthCryptKey1;//pck�е�ÿһ���ļ�����һ�����������ļ�β�������280�ֽڣ�
	uint32_t		IndexCompressedFilenameDataLengthCryptKey2;//����Ϊlen^key1, len^key2, data(PCKFILEINDEX), (11 11 11 11 22 22 22 22 dd dd dd dd .....)
	//pkx
	uint32_t		dwMaxSinglePckSize;
}PCK_KEYS, *LPPCK_KEYS;

typedef struct _PCK_VERSION_FUNC
{

	//const		PCK_KEYS*	cPckXorKeys;
	PCK_KEYS	cPckXorKeys;
	//ͷ��size
	size_t		dwHeadSize;
	//β��size
	size_t		dwTailSize;
	//fileindex��size
	size_t		dwFileIndexSize;
	//��ȡͷ
	//��ȡβ
	//��ȡfileindex
	BOOL(*PickIndexData)(void*, void*);
	//����ͷ
	void*(*FillHeadData)(void*);
	//����β
	void*(*FillTailData)(void*);
	//����fileindex
	void*(*FillIndexData)(void*, void*);
}PCK_VERSION_FUNC, *LPPCK_VERSION_FUNC;

//****** structures ******* 
#pragma pack(push, 4)

/*
** PCKHEAD PCK�ļ����ļ�ͷ�ṹ
** size = 12
**
** dwHeadCheckHead
** ���ļ���������Ϸ��أ���ͬ��Ϸ��ֵ��ͬ
**
** dwPckSize
** ����pck�ļ��Ĵ�С
**
** dwHeadCheckTail
** ���ļ��汾��2.0.2ʱ���ã����汾��2.0.3ʱ���ϲ���dwPckSize
** ����Ϊ��λʹ��
**
*/

typedef struct _PCK_HEAD_V2020
{
	uint32_t		dwHeadCheckHead;
	uint32_t		dwPckSize;
	uint32_t		dwHeadCheckTail;
}PCKHEAD_V2020, *LPPCKHEAD_V2020;

typedef struct _PCK_HEAD_V2030
{
	uint32_t		dwHeadCheckHead;
	union
	{
		uint64_t		dwPckSize;
		struct
		{
			uint32_t		dwPckSizeLow;
			uint32_t		dwHeadCheckTail;
		};
	};
}PCKHEAD_V2030, *LPPCKHEAD_V2030;

/*
** PCKTAIL PCK�ļ����ļ�β�����8�ֽڣ��ṹ
** size = 8
**
** dwFileCount
** �ļ���ѹ�����ļ�����
**
** dwVersion
** pck�ļ��İ汾
**
*/

typedef struct _PCK_TAIL_V2020
{
	uint32_t		dwIndexTableCheckHead;	//guardByte0
	uint32_t		dwVersion0;				//dwVersion
	uint32_t		dwEntryOffset;
	uint32_t		dwFlags;				//package flags. the highest bit means the encrypt state;, a5 != 0 ? 0x80000000 : 0 ���� a5 === 0
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint32_t		dwIndexTableCheckTail;	//guardByte1
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_V2020, *LPPCKTAIL_V2020;

typedef struct _PCK_TAIL_V2030
{
	uint32_t		dwIndexTableCheckHead;
	uint32_t		dwVersion0;
	uint64_t		dwEntryOffset;
	uint32_t		dwFlags;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint64_t		dwIndexTableCheckTail;
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_V2030, *LPPCKTAIL_V2030;

typedef struct _PCK_TAIL_VXAJH
{
	uint32_t		dwIndexTableCheckHead;
	union
	{
		uint64_t		dwEntryOffset;
		struct
		{
			uint32_t		dwVersion0;
			uint32_t		dwCryptedFileIndexesAddrHigh;
		};
	};
	uint32_t		dwFlags;
	char			szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	uint32_t		dwIndexTableCheckTail;
	uint32_t		dwFileCount;
	uint32_t		dwVersion;
}PCKTAIL_VXAJH, *LPPCKTAIL_VXAJH;

typedef struct _PCK_FILE_INDEX_V2020
{
	char			szFilename[MAX_PATH_PCK_256];
	uint32_t		dwUnknown1;
	uint32_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_V2020, *LPPCKFILEINDEX_V2020;

typedef struct _PCK_FILE_INDEX_V2030
{
	char			szFilename[MAX_PATH_PCK_260];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_V2030, *LPPCKFILEINDEX_V2030;


#if PCK_V2031_ENABLE
/*
������������С�ĳ���288���¼���4�ֽ�����
ĿǰӰ�첻����ʱ�����
*/
typedef struct _PCK_FILE_INDEX_V2031
{
	char			szFilename[MAX_PATH_PCK_260];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
	uint32_t		dwUnknown3;
}PCKFILEINDEX_V2031, *LPPCKFILEINDEX_V2031;

#endif

typedef struct _PCK_FILE_INDEX_VXAJH
{
	char			szFilename[MAX_PATH_PCK_256];
	uint32_t		dwUnknown1;
	uint64_t		dwAddressOffset;
	uint32_t		dwFileClearTextSize;
	uint32_t		dwFileCipherTextSize;
	uint32_t		dwUnknown2;
}PCKFILEINDEX_VXAJH, *LPPCKFILEINDEX_VXAJH;

typedef struct _PCK_FILE_INDEX
{
	wchar_t			szwFilename[MAX_PATH_PCK_260];		//ͨ��Unicode����
	char			szFilename[MAX_PATH_PCK_260];		//ʹ��pck�ڲ�ansi���룬Ĭ��CP936
	uint64_t		dwAddressOffset;
	ulong_t			dwFileClearTextSize;
	ulong_t			dwFileCipherTextSize;
}PCKFILEINDEX, *LPPCKFILEINDEX;
#pragma pack(pop)


typedef struct _PCK_INDEX_TABLE
{
	int				entryType;
	PCKFILEINDEX	cFileIndex;
	BOOL			isInvalid;			//����ļ�ʱ������ļ����ظ����򱻸��ǵ��ļ�����ΪTRUE���ڴ��ļ������ļ�Ŀ¼��ʱ��ͬ�����ļ�������ĻḲ��ǰ��ģ������ǵĴ�ֵ��ΪTRUE)
	BOOL			isProtected;		//�ļ����ñ�����������ɾ��
	BOOL			isToDeDelete;		//����ΪTRUE���ļ��ᱻɾ����isProtectedΪTRUE�ĳ���
	uint32_t		dwMallocSize;						//����ռ�ʹ�õĴ�С��>=ѹ������ļ���С��
	LPBYTE			compressed_file_data;				//��index��Ӧ��ѹ������
	size_t			nFilelenBytes;			//�ļ���(pck ansi)�����ֽ���
	size_t			nFilelenLeftBytes;		//�ļ���(pck ansi)ʣ������ֽ�����������ʱ�ã�ʹ��MAX_PATH_PCK_256
}PCKINDEXTABLE, *LPPCKINDEXTABLE;


typedef struct _PCK_PATH_NODE
{
	int				entryType;
	wchar_t			szName[MAX_PATH_PCK_260];
	uint32_t		dwFilesCount;
	uint32_t		dwDirsCount;
	size_t			nNameSizeAnsi;		//�ڵ�����pck ansi ����, ��..Ŀ¼��¼��Ŀ¼·������gfx\�µ�..Ŀ¼���ĳ��ȣ�ansi��
	size_t			nMaxNameSizeAnsi;	//
	uint64_t		qdwDirClearTextSize;
	uint64_t		qdwDirCipherTextSize;
	LPPCKINDEXTABLE	lpPckIndexTable;
	_PCK_PATH_NODE	*parentfirst;		//�Ǹ�Ŀ¼�µ�..Ŀ¼ʹ�ã�ָ��..Ŀ¼���ϼ�Ŀ¼�Ľڵ㣬Ҳ���Ǳ������Ŀ¼���ڵ�..Ŀ¼
	_PCK_PATH_NODE	*parent;			//�Ǹ�Ŀ¼�µ�..Ŀ¼ʹ�ã�ָ��Ŀ¼���ϼ�Ŀ¼�Ľڵ㣬Ҳ���Ǳ������Ŀ¼���ϼ�Ŀ¼
	_PCK_PATH_NODE	*child;				//��ͨĿ¼ָ���¼�Ŀ¼��..Ŀ¼�Ľڵ�
	_PCK_PATH_NODE	*next;				//����Ŀ¼ָ����һ��Ľڵ�
}PCK_PATH_NODE, *LPPCK_PATH_NODE;


typedef struct _FILES_TO_COMPRESS
{
#if PCK_DEBUG_OUTPUT
	int				id;
#endif
	uint32_t			dwCompressedflag;
	uint32_t			dwFileSize;
	uint32_t			nBytesToCopy;
	char			szFilename[MAX_PATH];
	wchar_t			szwFilename[MAX_PATH];
	size_t			nFileTitleLen;
	_FILES_TO_COMPRESS	*next;
	_PCK_INDEX_TABLE	*samePtr;
}FILES_TO_COMPRESS, *LPFILES_TO_COMPRESS;


typedef struct _PCK_INDEX_TABLE_COMPRESS
{
	union{
		BYTE			compressed_index_data[1];
		struct
		{
			uint32_t			dwIndexValueHead;
			uint32_t			dwIndexValueTail;
			BYTE			buffer[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
		};
	};
	ulong_t				dwIndexDataLength;					//�ļ�����ѹ����Ĵ�С
	uint32_t			dwCompressedFilesize;				//ѹ������ļ���С
	uint32_t			dwMallocSize;						//����ռ�ʹ�õĴ�С��>=ѹ������ļ���С��
	uint64_t			dwAddressFileDataToWrite;			//ѹ���������д���ļ��ĵ�ַ
	uint32_t			dwAddressOfDuplicateOldDataArea;	//���ʹ���������������ַ
}PCKINDEXTABLE_COMPRESS, *LPPCKINDEXTABLE_COMPRESS;


typedef struct _PCK_ALL_INFOS
{
	BOOL				isPckFileLoaded;	//�Ƿ��ѳɹ�����PCK�ļ� 
	uint64_t				qwPckSize;
	uint32_t				dwFileCount;
	uint64_t				dwAddressOfFileEntry;		//��ֵָ��pck�ļ���������ĩβ��Ҳ�����ļ�������ѹ�����ݵ���ʼλ��
	char				szAdditionalInfo[PCK_ADDITIONAL_INFO_SIZE];
	wchar_t				szFilename[MAX_PATH];
	wchar_t				szFileTitle[MAX_PATH];

	//std::vector<PCKINDEXTABLE> lstFileEntry;	//PCK�ļ�������
	LPPCKINDEXTABLE		lpPckIndexTable;	//PCK�ļ�������
	PCK_PATH_NODE		cRootNode;			//PCK�ļ��ڵ�ĸ��ڵ�

	wchar_t				szNewFilename[MAX_PATH];
	uint32_t				dwFileCountOld;
	uint32_t				dwFileCountToAdd;
	uint32_t				dwFinalFileCount;

	std::vector<FILES_TO_COMPRESS>				*lpFilesToBeAdded;
	const std::vector<PCKINDEXTABLE_COMPRESS>	*lpPckIndexTableToAdd;

	const PCK_VERSION_FUNC*	lpDetectedPckVerFunc;
	const PCK_VERSION_FUNC*	lpSaveAsPckVerFunc;

}PCK_ALL_INFOS, *LPPCK_ALL_INFOS;



class CPckControlCenter;



typedef struct _PCK_VARIETY_PARAMS {

	uint32_t		dwOldFileCount;
	uint32_t		dwPrepareToAddFileCount;
	uint32_t		dwChangedFileCount;
	uint32_t		dwDuplicateFileCount;
	uint32_t		dwFinalFileCount;

	//DWORD		dwUseNewDataAreaInDuplicateFileSize;
	uint32_t		dwDataAreaSize;
	uint32_t		dwRedundancyDataSize;

	LPCSTR		lpszAdditionalInfo;

	uint32_t		dwUIProgress;
	uint32_t		dwUIProgressUpper;

	uint32_t		dwMTMemoryUsed;

	BOOL		bThreadRunning;
	BOOL		bForcedStopWorking;	//ǿ��ֹͣ
	int			errMessageNo;			//0 - ok

}PCK_VARIETY_PARAMS;


typedef struct _PCK_RUNTIME_PARAMS {

	PCK_VARIETY_PARAMS	cVarParams;

	uint32_t		dwMTMaxMemory;		//���ʹ���ڴ�
	uint32_t		dwMTThread;			//ѹ���߳���
	uint32_t		dwCompressLevel;	//����ѹ����

	//int			code_page;			//pck�ļ�ʹ�ñ���

	CPckControlCenter	*lpPckControlCenter;

}PCK_RUNTIME_PARAMS, *LPPCK_RUNTIME_PARAMS;



#endif