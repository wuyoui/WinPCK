//////////////////////////////////////////////////////////////////////
// ZupHeader.h: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#if !defined(_ZUPHEADER_H_)
#define _ZUPHEADER_H_

//#define	TEXT_SORRY			"sorry"
#define	TEXT_NOTSUPPORT		"��ʱ��֧�ִ˹���"

typedef struct _ZUP_INC {
	uint16_t	wID;
	char		szMD5[34];
	uint8_t		abMD5[16];
	
	_ZUP_INC	*next;
}ZUPINC, *LPZUPINC;

typedef struct _ZUP_INC_LIST {

	uint16_t		wID;
	uint16_t		wMinVersion;
	uint16_t		wTargetVersion;
	uint32_t		dwTotalSize;
	char			szSingature[192];
	uint8_t			abSingature[128];
	LPZUPINC		lpZupIncFileList;

	_ZUP_INC_LIST	*next;

}ZUPINCLIST, *LPZUPINCLIST;



#endif