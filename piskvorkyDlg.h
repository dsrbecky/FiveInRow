// piskvorkyDlg.h : header file
//

#if !defined(AFX_PISKVORKYDLG_H__974C9F7C_0660_420F_AF51_3FC80AC29D75__INCLUDED_)
#define AFX_PISKVORKYDLG_H__974C9F7C_0660_420F_AF51_3FC80AC29D75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyDlg dialog
//#include "piskvorky.h"
struct MARKINFO
{
	int danger [5];
	int blocks [5][2];
	std::vector<int> needfor99[5][2];
	std::vector<int> needfor2[5][2];
	std::vector<int> needfor1[5][2];
};

struct INFO
{
	int mark;
	BOOL original;
	MARKINFO X,O;
	MARKINFO* Cur();
	MARKINFO* OpCur();
	MARKINFO* ByVal(int player);
};
typedef INFO INFOARR[spacewidth][spaceheight];

void RecalcInfo (int _u=-1,int _x=-1,int _y=-1,int _direction=-1,INFOARR i=0) ;
void RecalcInfoMain (int u,int x,int y,int direction,INFOARR pi);
UINT DoComputerMove( LPVOID pParam );
BOOL (* LRtoTable (int x,int y,int lr[5][2],int _direction=-1))[spaceheight];
BOOL PlayAt (int x,int y);
inline BOOL InSpace(INT x,INT y);
void SmartRecalcInfo (int x,int y,INFOARR i);
BOOL Validate  (int x,int y,INFOARR i,BOOL (*blocks)[spaceheight] = 0);

enum STATES
{
	playing,
	calculating,
	end
};	
struct SETUP
{
	CString skin;
};

class CPiskvorkyDlg : public CDialog
{
// Construction
public:
	
	CPoint GetSquare (CPoint point);
	void DoHummanMove (CPoint point);
	void NewGame ();
	CPiskvorkyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPiskvorkyDlg)
	enum { IDD = IDD_PISKVORKY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPiskvorkyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPiskvorkyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PISKVORKYDLG_H__974C9F7C_0660_420F_AF51_3FC80AC29D75__INCLUDED_)
