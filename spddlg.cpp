#include <afxwin.h>
#include "resource.h"
#include "spddlg.h"

CSpeedDlg::CSpeedDlg(CWnd* pParent):CDialog(IDD_SPEEDDLG,pParent)
{
	m_speed = 1;
}

void CSpeedDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	DDX_Text(pDX, IDC_SPEED, m_speed);
	DDV_MinMaxUInt(pDX, m_speed, 1, 10);
}






