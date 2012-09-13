#include <afxwin.h>
#include "resource.h"
#include "mainfrm.h"
#include "gendlg.h"
#include "spddlg.h"

BEGIN_MESSAGE_MAP(CMainFrame,CFrameWnd)

	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()

	ON_COMMAND(ID_START,OnStart)
	ON_COMMAND(ID_STOP,OnStop)
	ON_COMMAND(ID_CLEAR,OnClear)
	ON_COMMAND(ID_RANDOMCELLS,OnRandomCells)
	ON_COMMAND(ID_GENERATIONS,OnGenerations)
	ON_COMMAND(ID_SPEED,OnSpeed)
	ON_COMMAND(ID_ABOUT,OnAbout)

	ON_UPDATE_COMMAND_UI(ID_START, OnUpdateStartUI)
	ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStopUI)
	ON_UPDATE_COMMAND_UI(ID_CLEAR, OnUpdateClearUI)
	ON_UPDATE_COMMAND_UI(ID_RANDOMCELLS, OnUpdateRandomCellsUI)
	ON_UPDATE_COMMAND_UI(ID_GENERATIONS, OnUpdateGenerationsUI)
	ON_UPDATE_COMMAND_UI(ID_SPEED, OnUpdateSpeedUI)

END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{	 
	Create(NULL,"Conway's Life", WS_OVERLAPPED|WS_CLIPCHILDREN|WS_SYSMENU);

	for (UINT row=0; row<NUMBEROFROWS; ++row)
		for (UINT col=0; col<NUMBEROFCOLS; ++col)
			m_world[row][col]=DEAD;

	m_showdead = false;
	m_generations = 500;
	m_curGeneration = 0;
	m_speed = 10;

	m_pLive=new CPtrList;
	m_pDie=new CPtrList;
	m_pNextLive=new CPtrList;
	m_pNextDie=new CPtrList;
}

CMainFrame::~CMainFrame()
{
	delete m_pBitmap;

	ReleaseNodes(m_pLive);
	ReleaseNodes(m_pDie);
	ReleaseNodes(m_pNextLive);
	ReleaseNodes(m_pNextDie);

	delete m_pLive;
	delete m_pDie;
	delete m_pNextLive;
	delete m_pNextDie;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx=1005;
	cs.cy=740;

	BOOL returnCode=CFrameWnd::PreCreateWindow(cs);
	return returnCode;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CFrameWnd::OnCreate(lpCreateStruct);

	m_toolBar.Create(this);
	m_toolBar.LoadToolBar(IDR_TOOLBAR1);
	m_toolBar.SetBarStyle(m_toolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);

	static UINT indicators[]=
	{
		ID_SEPARATOR,
		ID_INDICATOR_SPEED,
		ID_INDICATOR_GENERATIONS,
		ID_INDICATOR_CURGENERATION
	};

	m_statusBar.Create(this);
	m_statusBar.SetIndicators(indicators, 4);
	m_statusBar.SetPaneInfo(0,m_statusBar.GetItemID(0),SBPS_STRETCH,0);
	m_statusBar.SetPaneText(1,"Speed: 10", TRUE);
	m_statusBar.SetPaneText(2,"Max Gens: 500", TRUE);
	m_statusBar.SetPaneText(3,"Generation: 0", TRUE);

	CClientDC clientDC(this);
	m_pBitmap=new CBitmap;
	m_pBitmap->CreateCompatibleBitmap(&clientDC,1005,740);
	ClearBitmap();
	DrawGrid();

	return 0;
}

void CMainFrame::OnPaint()
{
	CPaintDC paintDC(this);
	CDC memDC;
	memDC.CreateCompatibleDC(&paintDC);
	memDC.SelectObject(m_pBitmap);
	// paintDC.BitBlt(0,0,665,740,&memDC,0,0,SRCCOPY); //640,480&&&&&//932,711
	paintDC.BitBlt(0,0,1005,740,&memDC,0,0,SRCCOPY);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_curGeneration!=0)
		return;

	if(ClickInsideGrid(point))
	{
		UINT gridCol=(point.x - HOROFFSET)/SQUARESIZE;
		UINT gridRow=(point.y - VEROFFSET)/SQUARESIZE;

		if(m_world[gridRow][gridCol]==DEAD)
		{
			m_world[gridRow][gridCol]=ALIVE;
			DrawCell(gridCol,gridRow,TRUE);
		}
	}
}

void CMainFrame::ClearBitmap ()
{
	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC (&dc);
	memDC.SelectObject (m_pBitmap);
	CBrush brush(RGB(255,255,255));
	memDC.FillRect (CRect(0,0,1005,740),&brush);
}

void CMainFrame::DrawGrid ()
{
	CClientDC dc(this);
	CDC memDC;
	memDC.CreateCompatibleDC (&dc);
	memDC.SelectObject (m_pBitmap);

	for (int x=0;x<=NUMBEROFCOLS;++x)
	{
		memDC.MoveTo (SQUARESIZE*x+HOROFFSET,VEROFFSET);
		memDC.LineTo (SQUARESIZE*x+HOROFFSET, SQUARESIZE*NUMBEROFROWS+VEROFFSET+1);
	}

	for (int y=0;y<=NUMBEROFROWS;++y)
	{
		memDC.MoveTo (HOROFFSET,SQUARESIZE*y+VEROFFSET);
		memDC.LineTo (SQUARESIZE*NUMBEROFCOLS+HOROFFSET, SQUARESIZE*y+VEROFFSET);
	}
}

void CMainFrame::DrawCell (UINT col,UINT row,BOOL alive)
{
	CBrush* pBrush;
	CClientDC clientDC (this);
	CDC memDC;
	memDC.CreateCompatibleDC (&clientDC);
	memDC.SelectObject (m_pBitmap);

	if(alive)
		pBrush=new CBrush(RGB(255, 0, 0));
	else
	{	
		if (m_showdead)
			pBrush=new CBrush(RGB(200, 200, 200));
		else
			pBrush=new CBrush(RGB(255, 255, 255));
	}

	CBrush* pOldBrush1=memDC.SelectObject (pBrush);
	CBrush* pOldBrush2=clientDC.SelectObject (pBrush);

	UINT drawCol= col * SQUARESIZE + HOROFFSET;
	UINT drawRow= row * SQUARESIZE + VEROFFSET;

	memDC.Rectangle (drawCol,drawRow,drawCol+SQUARESIZE+1, drawRow+SQUARESIZE+1);
	clientDC.Rectangle (drawCol,drawRow,drawCol+SQUARESIZE+1, drawRow+SQUARESIZE+1);

	memDC.SelectObject (pOldBrush1);
	clientDC.SelectObject (pOldBrush2);

	delete pBrush;
}

BOOL CMainFrame::ClickInsideGrid (CPoint point)
{
	int gridSize=SQUARESIZE*NUMBEROFROWS;

	if ( (point.x<gridSize+HOROFFSET)&&(point.x>HOROFFSET)&&
		(point.y<gridSize+VEROFFSET)&&(point.y>VEROFFSET) )
		return TRUE;
	return FALSE;
}

bool CMainFrame::PopulationExists()
{
	bool populationExists = false;

	for (int c=0;c<NUMBEROFCOLS;++c)
		for(int r=0;r<NUMBEROFROWS;++r)
			if (m_world[r][c]==ALIVE)
			{
				populationExists = true;
				break;
			}
			return populationExists;
}

void CMainFrame::OnStart ()
{
	if (PopulationExists())
	{
		CreateLists();
		m_curGeneration = 1;
		SetTimer(1, 2010 - m_speed * 200, NULL);
	}
}

void CMainFrame::ReleaseNodes (CPtrList* pList)
{
	UINT count=pList->GetCount ();
	for (UINT i=0;i<count;++i)
	{
		CPoint* pCell=(CPoint*)pList->RemoveHead ();
		delete pCell;
	}
}

void CMainFrame::AddNbrs ()
{
	int xlow,xhigh,ylow,yhigh;
	int c,r;
	CPoint* pCell;

	while (!m_pLive->IsEmpty ())
	{
		pCell=(CPoint*)m_pLive->RemoveHead ();
		c=pCell->x;
		r=pCell->y;
		delete pCell;
		CalcLimits(c,r,xlow,xhigh,ylow,yhigh);

		for(int x=xlow;x<=xhigh;++x)
			for(int y=ylow;y<=yhigh;++y)
				if( (x!=c)||(y!=r) )
				{

					m_nbrs[y][x]+=1;
					switch(m_nbrs[y][x])
					{
					case 1:
					case 2:break;

					case 3:
						if(m_world[y][x]==DEAD)
						{
							pCell=new CPoint(x,y);
							m_pNextLive->AddTail (pCell);
						}
						break;

					case 4:
						if(m_world[y][x]==ALIVE)
						{
							pCell=new CPoint(x,y);
							m_pNextDie->AddTail (pCell);
						}
						break;

					case 5:
					case 6:
					case 7:
					case 8:break;
					}
				}
	}
}

void CMainFrame::CreateLists()
{
	int c,r;
	CPoint* pCell;

	ReleaseNodes(m_pLive);
	ReleaseNodes(m_pDie);
	ReleaseNodes(m_pNextLive);
	ReleaseNodes(m_pNextDie);

	// Suzdaveme nov spisuk s jivi kletki...
	for (c=0;c<NUMBEROFCOLS;++c)
		for(r=0;r<NUMBEROFROWS;++r)
		{
			m_nbrs[r][c]=0;
			if (m_world[r][c]==ALIVE)
			{
				pCell=new CPoint(c,r);
				m_pLive->AddTail (pCell);
			}
		}

		// Izchisliavame broia na susedite na tekushtata...
		AddNbrs();

		for (c=0;c<NUMBEROFCOLS;++c)
		{
			for(r=0;r<NUMBEROFROWS;++r)
			{
				if(((m_nbrs[r][c]<2)||(m_nbrs[r][c]>3)) && (m_world[r][c]==ALIVE) )
				{
					pCell=new CPoint(c,r);
					m_pNextDie->AddTail (pCell);
				}
			}
		}

		TransferList(&m_pLive,&m_pNextLive);
		TransferList(&m_pDie,&m_pNextDie);
}

void CMainFrame::TransferList (CPtrList** pDestList, CPtrList** pSrcList)
{
	ReleaseNodes(*pDestList);
	CPtrList *pTempList = *pDestList;
	*pDestList = *pSrcList;
	*pSrcList = pTempList;
}

void CMainFrame::OnTimer (UINT nIDEvent)
{
	++m_curGeneration;
	if(m_curGeneration>m_generations)
	{
		KillTimer(1);
		m_curGeneration=0;
		return;
	}

	char s[20];
	wsprintf(s,"Generation: %d",m_curGeneration);
	m_statusBar.SetPaneText (3,s,TRUE);

	// Izpulnenie na edno pokolenie...
	Live();
	Die();
	AddNbrs();
	SubNbrs();
	TransferList(&m_pLive,&m_pNextLive);
	TransferList(&m_pDie,&m_pNextDie);
}

void CMainFrame::Live ()
{
	int r,c;
	CPoint* pCell;

	CPtrList* pTempList=new CPtrList;
	TransferList(&pTempList, &m_pLive);

	while(!pTempList->IsEmpty ())
	{
		pCell=(CPoint*)pTempList->RemoveHead ();
		c=pCell->x;
		r=pCell->y;
		delete pCell;

		if((m_world[r][c]==DEAD)&&(m_nbrs[r][c]==3))
		{
			m_world[r][c]=ALIVE;
			DrawCell(c,r,TRUE);
			CPoint* pCell=new CPoint(c,r);
			m_pLive->AddTail (pCell);
		}
	}
	delete pTempList;
}

void CMainFrame::Die ()
{
	int r,c;
	CPoint* pCell;

	CPtrList* pTempList=new CPtrList;
	TransferList(&pTempList,&m_pDie);

	while(!pTempList->IsEmpty ())
	{
		pCell=(CPoint*)pTempList->RemoveHead ();
		c=pCell->x;
		r=pCell->y;
		delete pCell;

		if ((m_world[r][c]==ALIVE)&&(m_nbrs[r][c]!=2)&&(m_nbrs[r][c]!=3))
		{
			m_world[r][c]=DEAD;
			DrawCell(c,r,FALSE);
			CPoint* pCell=new CPoint(c,r);
			m_pDie->AddTail (pCell);
		}
	}
	delete pTempList;
}

void CMainFrame::SubNbrs ()
{
	int xlow,xhigh,ylow,yhigh;
	int c,r;
	CPoint* pCell;

	while (!m_pDie->IsEmpty ())
	{
		pCell=(CPoint*)m_pDie->RemoveHead ();
		c=pCell->x;
		r=pCell->y;
		delete pCell;
		CalcLimits(c,r,xlow,xhigh,ylow,yhigh);

		for(int x=xlow;x<=xhigh;++x)
			for(int y=ylow;y<=yhigh;++y)
				if( (x!=c)||(y!=r) )
				{
					m_nbrs[y][x] -= 1;
					switch(m_nbrs[y][x])
					{
					case 0:break;
					case 1:
						if (m_world[y][x]==ALIVE)
						{
							pCell=new CPoint(x,y);
							m_pNextDie->AddTail (pCell);
						}
						break;
					case 2:break;
					case 3:
						if(m_world[y][x]==DEAD)
						{
							pCell=new CPoint(x,y);
							m_pNextLive->AddTail (pCell);
						}
						break;

					case 4:
					case 5:
					case 6:
					case 7: break;
					}
				}
	}
}

void CMainFrame::OnClear ()
{
	for (UINT row=0;row<NUMBEROFROWS;++row)
		for (UINT col=0;col<NUMBEROFCOLS;++col)
			m_world[row][col]=DEAD;

	ClearBitmap();
	DrawGrid();
	Invalidate();
}

void CMainFrame::OnRandomCells ()
{
	srand ((unsigned)time(NULL));

	for (UINT cell=0;cell<1000;++cell)
	{
		UINT col=rand() % NUMBEROFCOLS;
		UINT row=rand() % NUMBEROFROWS;
		m_world[row][col]=ALIVE;
		DrawCell(col,row,TRUE);
	}
}

void CMainFrame::OnGenerations()
{
	CGenDlg dialog(this);
	dialog.m_generations = m_generations;
	dialog.m_showdead = m_showdead;
	int result=dialog.DoModal ();
	if (result==IDOK)
	{
		m_showdead = dialog.m_showdead;
		m_generations = dialog.m_generations;
		char s[20];
		wsprintf(s,"Max Gens: %d",m_generations);
		m_statusBar.SetPaneText (2,s,TRUE);
	}
}

void CMainFrame::OnSpeed()
{
	CSpeedDlg dialog(this);
	dialog.m_speed = m_speed;
	int result=dialog.DoModal ();
	if (result==IDOK)
	{
		m_speed=dialog.m_speed ;
		char s[20];
		wsprintf(s,"Speed: %d",m_speed);
		m_statusBar.SetPaneText (1,s,TRUE);
	}
}

void CMainFrame::OnStop ()
{
	KillTimer(1);
	m_curGeneration=0;
}

void CMainFrame::OnKeyDown (UINT nChar,UINT nRepCnt,UINT nFlags)
{
	if(m_curGeneration==0)
	{
		switch(nChar)
		{
		case VK_F2:
			OnStart();
			break;
		case VK_F4:
			OnClear();
			break;
		case VK_F5:
			OnRandomCells();
			break;
		case VK_F6:
			OnGenerations();
			break;
		case VK_F7:
			OnSpeed();
			break;
		case VK_F8:
			OnAbout();
		}
	}
	else
		if (nChar==VK_F3)
			OnStop();
}

void CMainFrame::OnDestroy ()
{
	KillTimer(1);
}

void CMainFrame::OnAbout ()
{
	CDialog dialog(MAKEINTRESOURCE(IDD_ABOUTDLG),this);
	dialog.DoModal ();
}

void CMainFrame::OnUpdateStartUI (CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_curGeneration==0);
}

void CMainFrame::OnUpdateStopUI (CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_curGeneration!=0);
}

void CMainFrame::OnUpdateClearUI (CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_curGeneration==0);
}

void CMainFrame::OnUpdateRandomCellsUI (CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_curGeneration==0);
}

void CMainFrame::OnUpdateGenerationsUI (CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_curGeneration==0);
}

void CMainFrame::OnUpdateSpeedUI (CCmdUI* pCmdUI)
{
	pCmdUI->Enable (m_curGeneration==0);
}

void CMainFrame::CalcLimits (int c,int r, int& xlow,int& xhigh,int& ylow,int& yhigh)
{
	if(c==0) xlow=0;
	else xlow=c-1;

	if(c==NUMBEROFCOLS-1) xhigh=NUMBEROFCOLS-1;
	else xhigh=c+1;

	if(r==0) ylow=0;
	else ylow=r-1;

	if(r==NUMBEROFROWS-1) yhigh=NUMBEROFROWS-1;
	else yhigh=r+1;
}
























