//////////////////////////////////////////////////////////////////////
// tSearchDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"

/*
���ҶԻ���
*/
BOOL TSearchDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_SEARCH, EM_LIMITTEXT, 255, 0);
	SetDlgItemTextW(IDC_EDIT_SEARCH, dirBuf);
	//SendDlgItemMessage(IDC_EDIT_SEARCH, EM_SETSEL, 0, -1);
	//::SetFocus(GetDlgItem(IDC_EDIT_SEARCH));
	return	TRUE;
}

BOOL TSearchDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		GetDlgItemTextW(IDC_EDIT_SEARCH, dirBuf, 256);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
