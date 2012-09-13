class CGenDlg:public CDialog
{
public:
	CGenDlg(CWnd* pParent);
public:
	UINT m_generations;
	BOOL m_showdead;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
};
