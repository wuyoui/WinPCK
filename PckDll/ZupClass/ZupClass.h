//////////////////////////////////////////////////////////////////////
// ZupClass.h: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"
#include "ZupHeader.h"
#include "DictHash.h"
#include "PckClassLog.h"

#if !defined(_ZUPCLASS_H_)
#define _ZUPCLASS_H_

class CZupClass : public CPckClass
{
public:

	//CZupClass();
	CZupClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CZupClass();

	BOOL	Init(LPCWSTR	szFile);

	const	LPPCKINDEXTABLE		GetPckIndexTable();

	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPCWSTR lpszScriptFile, LPCWSTR szRebuildPckFile, BOOL bUseRecompress) { Logger.e(TEXT_NOTSUPPORT);return FALSE; }
	virtual BOOL	StripPck(LPCWSTR lpszStripedPckFile, int flag) { Logger.e(TEXT_NOTSUPPORT); return FALSE; }

	//�½�������pck�ļ�
	virtual BOOL	UpdatePckFile(LPCWSTR szPckFile, const vector<wstring> &lpszFilePath, const PCK_PATH_NODE* lpNodeToInsert) { Logger.e(TEXT_NOTSUPPORT);return FALSE; }

	//�������ļ�
	virtual BOOL	RenameFilename() { Logger.e(TEXT_NOTSUPPORT);return FALSE; }

	//ɾ��һ���ڵ�
	virtual void	DeleteNode(LPPCK_PATH_NODE lpNode) { Logger.e(TEXT_NOTSUPPORT);}
	virtual void	DeleteNode(LPPCKINDEXTABLE lpIndex) { Logger.e(TEXT_NOTSUPPORT); }

	//������һ���ڵ�
	virtual	BOOL	RenameNode(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) { Logger.e(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCK_PATH_NODE lpNode, const wchar_t* lpszReplaceString) { Logger.e(TEXT_NOTSUPPORT); return FALSE; }
	virtual	BOOL	RenameIndex(LPPCKINDEXTABLE lpIndex, const wchar_t* lpszReplaceString) { Logger.e(TEXT_NOTSUPPORT); return FALSE; }

	//Ԥ���ļ�
	virtual BOOL	GetSingleFileData(const PCKINDEXTABLE* const lpZupFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

private:

	LPPCKINDEXTABLE				m_lpZupIndexTable;
	CDictHash					*m_lpDictHash;

private:

	void	BuildDirTree();
	BOOL	BuildZupBaseDict();
	void	DecodeFilename(char *_dst, wchar_t *_wdst, char *_src);

	_inline void	DecodeDict(LPZUP_FILENAME_DICT lpZupDict);
	//void	AddDict(char *&lpszStringToAdd);
	void	AddDict(std::string& base_file);

	const PCKINDEXTABLE* GetBaseFileIndex(const PCKINDEXTABLE* lpIndex, const PCKINDEXTABLE* lpZeroBaseIndex);

};

#endif