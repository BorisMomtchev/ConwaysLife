#include <afxwin.h>
#include "lifeapp.h"
#include "mainfrm.h"

CLifeApp LifeApp;

CLifeApp::CLifeApp()
{
}

BOOL CLifeApp::InitInstance()
{
	m_pMainWnd = new CMainFrame();
	m_pMainWnd -> ShowWindow(m_nCmdShow);
	m_pMainWnd -> UpdateWindow();
	return TRUE;
}