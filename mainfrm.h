#include <afxext.h>

const int SQUARESIZE=12;
const int NUMBEROFROWS=54;
const int NUMBEROFCOLS=82;
const int VEROFFSET=34;
const int HOROFFSET=4;
const int ALIVE=1;
const int DEAD=0;

class CMainFrame:public CFrameWnd
{
protected:
	CToolBar m_toolBar;
	CStatusBar m_statusBar;
	CBitmap* m_pBitmap;
	BOOL m_world[NUMBEROFROWS][NUMBEROFCOLS];
	UINT m_nbrs[NUMBEROFROWS][NUMBEROFCOLS];
	UINT m_curGeneration;
	UINT m_generations;
	BOOL m_showdead;
	UINT m_speed;
	CPtrList* m_pLive;
	CPtrList* m_pDie;
	CPtrList* m_pNextLive;
	CPtrList* m_pNextDie;
public:
	CMainFrame();
	~CMainFrame();
protected:
	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnClear();
	afx_msg void OnRandomCells();
	afx_msg void OnGenerations();
	afx_msg void OnSpeed();
	afx_msg void OnAbout();

	afx_msg void OnUpdateStartUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStopUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpeedUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGenerationsUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRandomCellsUI(CCmdUI* pCmdUI);

	void ClearBitmap();
	void DrawGrid();
	BOOL ClickInsideGrid(CPoint point);
	void DrawCell(UINT col, UINT row, BOOL alive);
	void CreateLists();
	void ReleaseNodes(CPtrList* pList);
	void Live();
	void Die();
	void AddNbrs();
	void SubNbrs();
	void CalcLimits(int c, int r, int &xlow, int &xhigh, int &ylow, int &yhigh);
	void TransferList(CPtrList** pDestList, CPtrList** pSrcList);
	bool PopulationExists();
	DECLARE_MESSAGE_MAP()
};




	



