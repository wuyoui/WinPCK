//////////////////////////////////////////////////////////////////////
// tStripDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴��뿪Դ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.12.29
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"
#include "PckModelStripDefines.h"

BOOL TStripDlg::EvCreate(LPARAM lParam)
{
	Show();
	return	FALSE;
}

BOOL TStripDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch (wID) {
	case IDOK:
		OnOK();
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

void TStripDlg::OnOK()
{
	int flag = PCK_STRIP_NONE;

	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_DDS) ? PCK_STRIP_DDS : PCK_STRIP_NONE);
	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ATT) ? PCK_STRIP_ATT : PCK_STRIP_NONE);
	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_GFX) ? PCK_STRIP_GFX : PCK_STRIP_NONE);
	flag |= (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_ECM) ? PCK_STRIP_ECM : PCK_STRIP_NONE);

	*pStripFlag = flag;
}