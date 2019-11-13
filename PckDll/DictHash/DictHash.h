//////////////////////////////////////////////////////////////////////
// CDictHash.h: ���ڽ����������繫˾��zup�ļ��е�����
// ����zup��base64�ֵ��ϣ��
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#ifndef _CDICTHASH_H_
#define _CDICTHASH_H_

#define	HASH_TABLE_LENGTH		0xffff
#define	MAX_PATH_PCK			260


typedef struct _ZUP_FILENAME_DICT {
	
	char				base64str[MAX_PATH_PCK];
	char				realbase64str[MAX_PATH_PCK];
	char				realstr[MAX_PATH_PCK];
	wchar_t				wrealstr[MAX_PATH_PCK];
	unsigned int		realstrlength;
	unsigned int		wrealstrlength;

	_ZUP_FILENAME_DICT	*next;
}ZUP_FILENAME_DICT, *LPZUP_FILENAME_DICT;

class CDictHash
{

public:
	CDictHash();
	virtual ~CDictHash();

	LPZUP_FILENAME_DICT	find(const char *keystr);

	LPZUP_FILENAME_DICT	add(const char *keystr);


protected:
	
	LPZUP_FILENAME_DICT	*lpDictHashTable;

	LPZUP_FILENAME_DICT	AllocNode(unsigned int sizeStuct);
	void	Dealloc(LPZUP_FILENAME_DICT lpDictHash);

};

#endif	//_CDICTHASH_H_