
#include "globals.h"
#include "pck_handle.h"
#include "miscdlg.h"
#include "tLogDlg.h"
#include <Shobjidl.h>

class TInstDlg : public TDlg
{
protected:
	TLogDlg			m_logdlg;

public:
	TInstDlg(LPTSTR cmdLine);
	virtual ~TInstDlg();

	virtual BOOL	EvCreate(LPARAM lParam);
	virtual BOOL	EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl);
	virtual BOOL	EvClose();
	virtual BOOL	EvTimer(WPARAM timerID, TIMERPROC proc);
	virtual BOOL	EvNotify(UINT ctlID, NMHDR *pNmHdr);
#ifdef _USE_CUSTOMDRAW_
	virtual BOOL	EvMeasureItem(UINT ctlID, MEASUREITEMSTRUCT *lpMis);
	virtual BOOL	EvDrawItem(UINT ctlID, DRAWITEMSTRUCT *lpDis);
#endif

	virtual BOOL	EvDropFiles(HDROP hDrop);

	virtual BOOL	EventButton(UINT uMsg, int nHitTest, POINTS pos);
	virtual BOOL	EvMouseMove(UINT fwKeys, POINTS pos);

	virtual BOOL	EvSize(UINT fwSizeType, WORD nWidth, WORD nHeight);

//�û�����
private:

	TCHAR	m_MyFileName[MAX_PATH];
	TCHAR	m_Filename[MAX_PATH], m_CurrentPath[MAX_PATH];

	BOOL	m_isListviewRenaming;

	int		m_iListHotItem;	//��ǰ�����listview�ϵ�ѡ��������

	const PCK_UNIFIED_FILE_ENTRY*	m_currentNodeOnShow;

	BOOL	bGoingToExit;
	wchar_t	m_szStrToSearch[256];

	//addmode
	vector<wstring>	m_lpszFilePath;

	//�����Ҵ��ڵı���
	BOOL	m_isSearchWindow;

	HCURSOR	m_hCursorOld, m_hCursorAllow, m_hCursorNo;

	wchar_t		m_FolderBrowsed[MAX_PATH];

	HIMAGELIST	m_imageList;
	_inline BOOL EnableButton(UINT buttonID, BOOL enable) { return (BOOL)(SendDlgItemMessage(IDC_TOOLBAR, TB_ENABLEBUTTON, buttonID, MAKELONG(enable, 0))); }

	//Timer String
	wchar_t		szTimerProcessingFormatString[64];
	wchar_t		szTimerProcessedFormatString[64];

	//����������
	ITaskbarList3* m_pTaskBarlist = nullptr;

	//�û�����
private:

	//winmain.cpp

	BOOL IsValidWndAndGetPath(wchar_t * szPath, BOOL isGetPath = FALSE);
	void RefreshProgress();
	TCHAR* BuildSaveDlgFilterString();


	//threadproc.cpp
	void EnbaleButtons(int ctlExceptID, BOOL bEnbale);
	static VOID UpdatePckFile(VOID *pParam);
	static VOID RenamePckFile(VOID *pParam);
	static VOID RebuildPckFile(VOID	*pParam);
	static VOID StripPckFile(VOID *pParam);
	static VOID CreateNewPckFile(VOID *pParam);
	static VOID ToExtractAllFiles(VOID *pParam);
	static VOID ToExtractSelectedFiles(VOID	*pParam);
	static VOID DeleteFileFromPckFile(VOID *pParam);


	//mainfunc.cpp
	BOOL OpenPckFile(wchar_t *lpszFileToOpen = L"", BOOL isReOpen = FALSE);
	VOID SearchPckFiles();
	VOID ShowPckFiles(const PCK_UNIFIED_FILE_ENTRY* lpNodeToShow);

#pragma region guilated.cpp
private:
	//guilated.cpp
	void		initCommctrls();
	void		initParams();
	void		initToolbar();
	void		initArgument();
#pragma endregion

#pragma region helpfunc.cpp
private:
	//helpfunc.cpp
	VOID ViewFileAttribute();
	VOID ViewFile(const PCK_UNIFIED_FILE_ENTRY* lpFileEntry);
	BOOL AddFiles();
	void AddSetupReg();
	void DeleteSetupReg();

	void InitLogWindow();

	const PCK_UNIFIED_FILE_ENTRY* GetFileEntryByItem(int itemIndex);

	void DbClickListView(const int itemIndex);	//�����б��е�itemIndex�����Ŀ¼��Ԥ���ļ���
	void PopupRightMenu(const int itemIndex);		//listview���һ����˵�

	void UnpackAllFiles();					//��ѹ�����ļ�
	void UnpackSelectedFiles();				//��ѹѡ�е��ļ�

#pragma endregion
private:
	//�򿪡��رա���ԭ���¼�ע��
	static int MyFeedbackCallback(void* pTag, int32_t eventId, size_t wParam, ssize_t lParam);


	//listViewFunc.cpp

	BOOL EvNotifyListView(const NMHDR *pNmHdr);
	BOOL EvDrawItemListView(const DRAWITEMSTRUCT *lpDis);

public:
	void InsertList(CONST HWND hWndList, CONST INT iIndex, CONST UINT uiMask, CONST INT iImage, CONST LPVOID lParam, CONST INT nColCount, ...);
private:
	BOOL InitListView(CONST HWND hWndListView, const LPTSTR *lpszText, const int *icx, const int *ifmt);

	BOOL ListView_BeginLabelEdit(const HWND hWndList, LPARAM lParam);
	BOOL ListView_EndLabelEdit(const NMLVDISPINFO* pNmHdr);

	void ListView_Init();
	void ListView_Uninit();

	void ProcessColumnClick(CONST HWND hWndList, CONST NMLISTVIEW * pnmlistview, DWORD& dwSortStatus);


#pragma region MenuButtonFuncs.cpp
	//menu
private:
	void MenuStrip();
	void MenuClose();
	void MenuInfo();
	void MenuSearch();
	void MenuNew(WORD wID);
	void MenuAdd(WORD wID);
	void MenuRebuild(WORD wID);
	void MenuCompressOpt();
	void MenuRename();
	void MenuDelete();
	void MenuSelectAll();
	void MenuSelectReverse();
	void MenuAttribute();
	void MenuView();
	void MenuAbout();
	void ListViewEnter();
#pragma endregion

#pragma region mainControlStatus.cpp

private:
	void SetStatusBarText(int iPart, LPCSTR lpszText);
	void SetStatusBarText(int iPart, LPCWSTR lpszText);

public:
	void SetStatusBarTitle(LPCWSTR lpszText);
	void SetStatusBarFileSize(uint64_t size);
	void SetStatusBarFileCount(uint32_t size);
	void ClearStatusBarProgress();
	void SetStatusBarProgress(LPCWSTR lpszText);
	void SetStatusBarInfo(LPCWSTR lpszText);

#pragma endregion


};

class TInstApp : public TApp
{
public:
	TInstApp(HINSTANCE _hI, LPTSTR _cmdLine, int _nCmdShow);
	virtual ~TInstApp();

	void InitWindow(void);
};