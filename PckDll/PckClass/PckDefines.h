//////////////////////////////////////////////////////////////////////
// PckDefines.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.13
//////////////////////////////////////////////////////////////////////

#include "pck_dependencies.h"
#include <limits.h>

#define __UCSTEXT(quote) L##quote
#define UCSTEXT(quote) __UCSTEXT(quote)


#if !defined(_PCKDEFINES_H_)
#define _PCKDEFINES_H_

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4005 )
#pragma warning ( disable : 4995 )

typedef enum _FMTPCK
{
	FMTPCK_PCK = 0,
	FMTPCK_ZUP = 1,
	FMTPCK_CUP = 2,
	FMTPCK_UNKNOWN = 0x7fffffff
}FMTPCK;

typedef enum _PCKVER
{
	PCK_V2020,
	PCK_V2030,
	PCK_VXAJH
}PCK_CATEGORY;

//LOG INFO STRING
#define TEXT_LOG_OPENFILE				"���ļ� %s"
#define TEXT_LOG_CLOSEFILE				"�ر��ļ�"
#define	TEXT_LOG_FLUSH_CACHE			"д�뻺����..."
#define	TEXT_LOG_RESTOR_OK				"���ݻָ��ɹ�"

#define TEXT_LOG_WORKING_DONE			"�������"

#define	TEXT_LOG_LEVEL_THREAD			"ѹ����=%d:�߳�=%d"

#define	TEXT_LOG_UPDATE_ADD				"����ģʽ"
#define	TEXT_LOG_UPDATE_NEW				"�½�ģʽ"

#define	TEXT_LOG_RENAME					"������(ɾ��)�����ļ�..."
#define	TEXT_LOG_REBUILD				"�ؽ�PCK�ļ�..."
#define	TEXT_LOG_RECOMPRESS				"��ѹ��PCK�ļ�..."


#define	TEXT_LOG_CREATE					"�½�PCK�ļ�:%s..."

#define	TEXT_LOG_COMPRESSOK				"ѹ����ɣ�д������..."

#define	TEXT_LOG_EXTRACT				"��ѹ�ļ�..."



//ERROR STRING
#define	TEXT_ERROR						"����"

#define TEXT_INVALID_VERSION			"��Ч�İ汾ID��"

#define	TEXT_MALLOC_FAIL				"�����ڴ�ʧ�ܣ�"
#define	TEXT_CREATEMAP_FAIL				"�����ļ�ӳ��ʧ�ܣ�"
#define	TEXT_CREATEMAPNAME_FAIL			"ӳ���ļ�\"%s\"ʧ�ܣ�"
#define	TEXT_OPENNAME_FAIL				"���ļ�\"%s\"ʧ�ܣ�"
#define	TEXT_VIEWMAP_FAIL				"����ӳ����ͼʧ�ܣ�"
#define	TEXT_VIEWMAPNAME_FAIL			"�ļ�\"%s\"����ӳ����ͼʧ�ܣ�"

#define TEXT_READ_INDEX_FAIL			"�ļ��������ȡ����"
#define TEXT_UNKNOWN_PCK_FILE			"�޷�ʶ���PCK�ļ���"

#define	TEXT_OPENWRITENAME_FAIL			"��д���ļ�\"%s\"ʧ�ܣ�"
#define	TEXT_READFILE_FAIL				"�ļ���ȡʧ�ܣ�"
#define	TEXT_WRITEFILE_FAIL				"�ļ�д��ʧ�ܣ�"
#define TEXT_WRITE_PCK_INDEX			"д���ļ���������"

#define	TEXT_USERCANCLE					"�û�ȡ���˳���"

#define	TEXT_COMPFILE_TOOBIG			"ѹ���ļ�����"
#define	TEXT_UNCOMP_FAIL				"��ѹ�ļ�ʧ�ܣ�"

#define	TEXT_UNCOMPRESSDATA_FAIL		"�ļ� %s \r\n���ݽ�ѹʧ�ܣ�"
#define	TEXT_UNCOMPRESSDATA_FAIL_REASON	"���ݽ�ѹʧ��: %s"

#define	TEXT_ERROR_OPEN_AFTER_UPDATE	"��ʧ�ܣ��������ϴεĲ����������ļ����𻵡�\r\n�Ƿ��Իָ����ϴδ�״̬��"
#define	TEXT_ERROR_GET_RESTORE_DATA		"��ȡ�ָ���Ϣʱ����"
#define	TEXT_ERROR_RESTORING			"�ָ�ʱ����"
#define	TEXT_ERROR_DUP_FOLDER_FILE		"���ڣ��ļ���=�ļ����������˳���"

#define TEXT_UPDATE_FILE_INFO			"Ԥ����ļ���=%d:Ԥ���ļ���С=%lld, ��ʼ������ҵ..."

#define TEXT_PCK_SIZE_INVALID			"��Ч��PCK�ļ���С"
#define TEXT_VERSION_NOT_FOUND			"û�ҵ���Ч�İ汾"

#endif