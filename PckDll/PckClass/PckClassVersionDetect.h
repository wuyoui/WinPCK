//////////////////////////////////////////////////////////////////////
// PckClassVersionDetect.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �汾��صļ���Լ����ݵ�д�롢��ȡ
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////
#pragma once
#include "PckClassBaseFeatures.h"

//PckVersion
#ifndef _DEBUG

#define PCK_VERSION_START_ID		0
#define	PCK_VERSION_ZX				0
#define	PCK_VERSION_ZXNEW			1
#define	PCK_VERSION_SDS				2
#define	PCK_VERSION_SDSNEW			3
#define PCK_VERSION_SM				4
#define PCK_VERSION_SMNEW			5
#define PCK_VERSION_KDXY			6
#define PCK_VERSION_KDXYNEW			7
#define PCK_VERSION_RWPD			8
#define PCK_VERSION_RWPDNEW			9
#define PCK_VERSION_END_ID			9
#define PCK_VERSION_NORMAL_NUMS		10
#define PCK_VERSION_SP_START_ID		10
#define	PCK_VERSION_XAJH			10
#define PCK_VERSION_SP_END_ID		10

#else

#define PCK_VERSION_START_ID		0
#define	PCK_VERSION_ZX				0
#define	PCK_VERSION_ZXNEW			1
#define	PCK_VERSION_SDS				2
#define	PCK_VERSION_SDSNEW			3
#define PCK_VERSION_SM				4
#define PCK_VERSION_SMNEW			5
#define PCK_VERSION_END_ID			5
#define PCK_VERSION_NORMAL_NUMS		6
#define PCK_VERSION_SP_START_ID		6
#define	PCK_VERSION_XAJH			6
#define PCK_VERSION_SP_END_ID		6

#endif

#define PCK_VERSION_INVALID		(-1)

#define MAX_SEARCH_DEPTH		10000

#define MAX_FILE_PACKAGE		0x7fffff00U

#define AFPCK_VERSION_202		0x00020002
#define AFPCK_VERSION_203		0x00020003

#define AFPCK_SAFEHEAFER_TAG1	0x4DCA23EF
#define AFPCK_SAFEHEAFER_TAG2	0x56A089B7

typedef PCKHEAD_V2030 PCKHEAD_VXAJH, *LPPCKHEAD_VXAJH;


static vector<PCK_VERSION_FUNC>	cPckVersionFunc;

typedef struct _PCK_VERSION_ID
{
	int			id;
	wchar_t		name[64];
	PCK_CATEGORY	VersionId;
	DWORD		Version;
	DWORD		AlgorithmId;
}PCK_VERSION_ID, *LPPCK_VERSION_ID;

class CPckClassVersionDetect :
	private virtual CPckClassBaseFeatures
{
public:
	CPckClassVersionDetect();
	~CPckClassVersionDetect();

	//���ð汾
	const	PCK_KEYS*	GetPckVersion();
	BOOL	SetSavePckVersion(int verID);
	static	const	wchar_t*	GetPckVersionNameById(int id);
	static	uint32_t	GetPckVersionCount();

	static	int		AddPckVersion(int AlgorithmId, int Version);

protected:
	//���pck�İ汾��д�뵽iDetectedPckID��ͬʱ��ȡ�ļ�ͷ��β
	BOOL	DetectPckVerion(LPCWSTR lpszPckFile);

private:

	static const PCK_VERSION_ID			cPckIDs[];
	static const PCK_KEYS				cPckSPKeys[];
	static const PCK_VERSION_FUNC		cPckSPVersionFunc[];

	//���汾��Ϣ
	static void		FillGeneralVersionInfo();
	static void		FillSpecialVersionInfo();
	static int		FillUnknownVersionInfo(DWORD AlgorithmId, DWORD Version);

	//PCK�汾�ж�
	static void		SetAlgorithmId(DWORD id, LPPCK_VERSION_FUNC lpPckVersionFunc);

	//�ļ�ͷ��β��������д������д��
	static void*	FillHeadData_V2020(void *param);
	static void*	FillHeadData_V2030(void *param);

	static void*	FillTailData_V2020(void *param);
	static void*	FillTailData_V2030(void *param);
	static void*	FillTailData_VXAJH(void *param);

	static void*	FillIndexData_V2020(void *param, void *pckFileIndexBuf);
	static void*	FillIndexData_V2030(void *param, void *pckFileIndexBuf);
	static void*	FillIndexData_VXAJH(void *param, void *pckFileIndexBuf);

	//���ݴ�lpIndex -> param
	static BOOL		PickIndexData_V2020(void *param, void* lpIndex);
	static BOOL		PickIndexData_V2030(void *param, void* lpIndex);
	static BOOL		PickIndexData_VXAJH(void *param, void* lpIndex);

	void	PrintInvalidVersionDebugInfo(const wchar_t * lpszPckFile);


};
