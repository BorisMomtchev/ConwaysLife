#include <afxwin.h>
#include "resource.h"
#include "gendlg.h"

CGenDlg::CGenDlg(CWnd* pParent):CDialog(IDD_GENERATIONSDLG,pParent)
{
	m_generations = 500;
}

void CGenDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	DDX_Text(pDX,IDC_GENERATIONS, m_generations);
	DDX_Check(pDX,IDC_SHOWDEAD, m_showdead);
	DDV_MinMaxUInt(pDX, m_generations, 1, 64000);
}
