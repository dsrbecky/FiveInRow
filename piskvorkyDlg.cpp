// piskvorkyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "piskvorky.h"
#include "piskvorkyDlg.h"
#include "SetupDlg.h"
 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
INFOARR info;
//INFO (*virt)[spaceheight];

int m_animID [spacewidth][spaceheight];
int m_phase [spacewidth][spaceheight];

CBitmap **m_bmps[7];
int m_bmpsCount[7];
int m_bmpxsize,m_bmpysize;
HCURSOR m_cursor;

HANDLE calculatingThread;
CCriticalSection cs_anim,cs_state;

SETUP m_setup;
int Player;
static STATES m_status;
int lastX, lastY;

MARKINFO* INFO::Cur()
{
	if (Player == 1)
		return &X;
	return &O;
}

MARKINFO* INFO::OpCur()
{
	if (Player == 1)
		return &O;
	return &X;
}
MARKINFO* INFO::ByVal(int player)
{
	if (player == 1)
		return &X;
	return &O;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyDlg dialog

CPiskvorkyDlg::CPiskvorkyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPiskvorkyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPiskvorkyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPiskvorkyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPiskvorkyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPiskvorkyDlg, CDialog)
	//{{AFX_MSG_MAP(CPiskvorkyDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPiskvorkyDlg message handlers

BOOL CPiskvorkyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	SetTimer(1,40,NULL);

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	ASSERT((IDM_SETUP & 0xFFF0) == IDM_SETUP);
	ASSERT(IDM_SETUP < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_NEWGAME,CString((LPCSTR)IDS_MNUNEWGAME));
			pSysMenu->AppendMenu(MF_STRING, IDM_SETUP,CString((LPCSTR)IDS_SETUP));
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	RECT rect;
	GetWindowRect (&rect);
	MoveWindow (rect.left,rect.top,spacewidth*m_bmpxsize+GetSystemMetrics(SM_CXFIXEDFRAME)*2,spaceheight*m_bmpysize+GetSystemMetrics(SM_CYFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPiskvorkyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CAboutDlg dlgAbout;
	switch (nID & 0xFFF0)
	{
	case IDM_ABOUTBOX:
		dlgAbout.DoModal();
		break;
	case IDM_SETUP:
		ShowSetupDlg ();
		break;
	case IDM_NEWGAME:
		if (IDYES==MessageBox (CString((LPCSTR)IDS_NEWGAMEMSG),CString((LPCSTR)IDS_NEWGAME),MB_YESNO))//IDYES
			NewGame ();
		break;
	default:
		CDialog::OnSysCommand(nID, lParam);
		break;
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPiskvorkyDlg::OnPaint() 
{
	CClientDC clientDC(this);
	CDC tempDC;	tempDC.CreateCompatibleDC (NULL);
	CBitmap* tempbmp=0;

#ifdef _DEBUG
	//RecalcInfo();
#endif

	cs_anim.Lock ();
	for (int x = 0;x<spacewidth;x++)
		for (int y = 0;y <spaceheight;y++)
			if (m_bmpsCount[m_animID[x][y]] != 0)
			{	
				CBitmap* sourcebmp=m_bmps[m_animID[x][y]][m_phase[x][y]];
				cs_anim.Unlock ();
				tempDC.SelectObject (sourcebmp);
				clientDC.BitBlt(m_bmpxsize*x,m_bmpysize*y,30,30,&tempDC,0,0,SRCCOPY);
				#ifdef _DEBUG
					CString tempstr;
					if (info !=0)tempstr.Format ("%d",info[x][y].Cur()->danger[0]);// <<-- DEBUG NUMBERS
					//tempstr.Format ("%d-%d",info[x][y].Cur()->madeby[2][0],info[x][y].Cur()->madeby[2][1]);// <<-- DEBUG NUMBERS
					clientDC.TextOut (m_bmpxsize*x,m_bmpysize*y,tempstr);
				#endif
				cs_anim.Lock ();
			}
	cs_anim.Unlock ();

	tempDC.SelectObject (tempbmp);	
	
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPiskvorkyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPiskvorkyDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		CClientDC clientDC(this);
		CDC tempDC;	tempDC.CreateCompatibleDC (NULL);
		CBitmap* tempbmp=0;
		cs_anim.Lock ();
		
		for (int x = 0;x<spacewidth;x++)
			for (int y = 0;y <spaceheight;y++)
			{
				if (++m_phase[x][y] >= m_bmpsCount[m_animID[x][y]]) 
					if ((m_animID[x][y]==1)|(m_animID[x][y]==2))
					{
						m_animID[x][y] += 2;
						m_phase[x][y]=0;
						goto repaint;
					}
					else m_phase[x][y]=0;
					if (m_bmpsCount[m_animID[x][y]] != 1)
					{
repaint:
						if (m_bmpsCount[m_animID[x][y]] != 0)
						{
							CBitmap* sourcebmp=m_bmps[m_animID[x][y]][m_phase[x][y]];
							cs_anim.Unlock ();
							tempDC.SelectObject (sourcebmp);
							clientDC.BitBlt(m_bmpxsize*x,m_bmpysize*y,30,30,&tempDC,0,0,SRCCOPY);
							cs_anim.Lock ();
#ifdef _DEBUG
							if (AfxGetApp()->m_pMainWnd != 0)
								Invalidate (FALSE);
#endif
						}
					}
			}
		cs_anim.Unlock ();
		tempDC.SelectObject (tempbmp);	
	}	

	CDialog::OnTimer(nIDEvent);
}


void CPiskvorkyDlg::NewGame()
{
	if (calculatingThread != 0)	
	{
		::TerminateThread (calculatingThread,999);
		calculatingThread = 0;
	}
//	virt = 0;
	m_status = playing;
    lastX = 7;
    lastY = 7;
	ZeroMemory (&info,sizeof(info));
	ZeroMemory (&m_animID,sizeof(m_animID));
	ZeroMemory (&m_phase,sizeof(m_phase));

    Player = 1;

	if (AfxGetApp()->m_pMainWnd != 0)
		AfxGetApp()->m_pMainWnd->Invalidate (FALSE);
}
BOOL CPiskvorkyDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_cursor !=0)
	{
		SetCursor (m_cursor);
		return -1;
	}
	else return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

UINT Calculation( LPVOID pParam );

CPoint mousedownpoint;

void CPiskvorkyDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	cs_state.Lock ();
		int tempstate = m_status;
	cs_state.Unlock ();

#ifdef _DEBUG
	CClientDC clientDC(this);
	CString tempstr;
//	tempstr.Format ("(%d)",GetSameDependings(mousedownpoint.x,mousedownpoint.y,GetSquare (point).x,GetSquare (point).y,m_depending));
	clientDC.TextOut (m_bmpxsize*GetSquare (point).x,m_bmpysize*GetSquare (point).y,tempstr);
#endif

	if (mousedownpoint == GetSquare (point)) 
	switch (tempstate)
	{
	case playing:
		DoHummanMove(GetSquare (point));
		break;
	case calculating:
		MessageBeep (MB_ICONEXCLAMATION);
		break;
	case end:
		if (IDYES==MessageBox (CString((LPCSTR)IDS_NEWGAMEMSG),CString((LPCSTR)IDS_NEWGAME),MB_YESNO))//IDYES
			NewGame ();
		break;
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

void CPiskvorkyDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
    if (IDYES==MessageBox (CString((LPCSTR)IDS_NEWGAMEMSG),CString((LPCSTR)IDS_NEWGAME),MB_YESNO))//IDYES
	    NewGame ();
    
	/*int x=GetSquare (point).x,y=GetSquare (point).y;
	TRACE2("danger at (%d,%d)\n",x,y);
	TRACE1("[0]  %d ",info[x][y].Cur()->danger[0]);
	TRACE1("[1]/ %d ",info[x][y].Cur()->danger[1]);
	TRACE1("[2]- %d ",info[x][y].Cur()->danger[2]);
	TRACE1("[3]\\ %d ",info[x][y].Cur()->danger[3]);
	TRACE1("[4]| %d\n",info[x][y].Cur()->danger[4]);*/
	
	CDialog::OnRButtonUp(nFlags, point);
}


void CPiskvorkyDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCursor (m_cursor);
	mousedownpoint = GetSquare (point);
	CDialog::OnLButtonDown(nFlags, point);
}

CPoint CPiskvorkyDlg::GetSquare(CPoint point)
{
	return CPoint (point.x / m_bmpxsize,point.y / m_bmpysize);
}

void CPiskvorkyDlg::DoHummanMove(CPoint point)
{
//  Check empty
	if (info[point.x][point.y].mark != 0)
	{
		m_status = playing;
		MessageBeep (MB_ICONEXCLAMATION);
		return;
	}
//  HUMMAN move
	if (PlayAt(point.x,point.y) == TRUE) return; // game over

//  Execute AI calculation
	m_status = calculating;
	CWinThread* calcthread;
	calcthread = AfxBeginThread (DoComputerMove,0 ,THREAD_PRIORITY_BELOW_NORMAL,0,CREATE_SUSPENDED);
	calculatingThread = calcthread->m_hThread;
	calcthread->ResumeThread ();
}

BOOL PlayAt(int x, int y)
{
	BOOL result=FALSE;
	cs_anim.Lock ();
		info[x][y].mark = Player;
		m_animID [x][y] = Player;
		m_phase [x][y] = 0;
        lastX = x;
        lastY = y;
	cs_anim.Unlock (); 

	for (int direction=1;direction<= 4;direction++) // win test
	{
		int plusx,plusy,left=-1,right=-1;
		switch (direction)
		{
		case 1:plusx=1;plusy=-1;break;
		case 2:plusx=1;plusy=0;break;
		case 3:plusx=1;plusy=1;break;
		case 4:plusx=0;plusy=1;break;
		}
		int srcx=x,srcy=y;
		do
		{
			srcx -= plusx;srcy -= plusy;
			left++;
		} while ((InSpace(srcx,srcy)&&(info[srcx][srcy].mark == Player)));
		srcx=x,srcy=y;
		do
		{
			srcx += plusx;srcy += plusy;
			right++;
		} while ((InSpace(srcx,srcy)&&(info[srcx][srcy].mark == Player)));
		if ((left+1+right)>=5)
		{
			cs_anim.Lock ();
			srcx=x,srcy=y;
			do
			{
				info[srcx][srcy].mark = Player;
				m_animID[srcx][srcy] = Player+4;
				m_phase [srcx][srcy] = 0;
				srcx -= plusx;srcy -= plusy;
				left++;
			} while ((InSpace(srcx,srcy)&&(info[srcx][srcy].mark == Player)));
			srcx=x,srcy=y;
			do
			{
				info[srcx][srcy].mark = Player;
				m_animID[srcx][srcy] = Player+4;
				m_phase [srcx][srcy] = 0;
				srcx += plusx;srcy += plusy;
				right++;
			} while ((InSpace(srcx,srcy)&&(info[srcx][srcy].mark == Player)));
			cs_anim.Unlock ();
			cs_state.Lock();
			m_status = end;
			cs_state.Unlock ();
			if (AfxGetApp()->m_pMainWnd != 0)
				AfxGetApp()->m_pMainWnd->Invalidate (FALSE);
			result= TRUE;
		}
	}
	Player = OpPlayer;
	return result;
}

// Check where is point
inline BOOL InSpace(INT x,INT y)
{
	return ((x >= 0)&&(x <= (spacewidth-1)))&&((y >= 0)&&(y <= (spaceheight-1)));
}


UINT DoComputerMove( LPVOID pParam )
{
        SmartRecalcInfo (lastX,lastY,0); // human recalc

		int newx=7,newy=7;

		int maxc=-1;// Find the bigest common danger
        int maxd=-1;// Find the bigest defensive danger
        int minDist=9999; // Find the the closest one
		for (int x = 0;x<spacewidth;x++)
			for (int y = 0;y <spaceheight;y++)
			{
                if (info[x][y].mark != 0) continue;
				if (
                    (info[x][y].OpCur()->danger[0] + info[x][y].Cur()->danger[0] > maxc)
                    ||
                    (
                     (info[x][y].OpCur()->danger[0] + info[x][y].Cur()->danger[0] == maxc) &&
                     (info[x][y].OpCur()->danger[0] > maxd)
                    )
                    ||
                    (
                     (info[x][y].OpCur()->danger[0] + info[x][y].Cur()->danger[0] == maxc) &&
                     (info[x][y].OpCur()->danger[0] == maxd) &&
                     ( ((x-lastX)*(x-lastX)+(y-lastY)*(y-lastY)) < minDist)
                    )
                   )
				{
					newx = x;
					newy = y;
					maxc = info[x][y].OpCur()->danger[0] + info[x][y].Cur()->danger[0];
                    maxd = info[x][y].OpCur()->danger[0];
                    minDist = ((x-lastX)*(x-lastX)+(y-lastY)*(y-lastY));
				}
			}

#ifdef _DEBUG
		if (AfxGetApp()->m_pMainWnd != 0)
			AfxGetApp()->m_pMainWnd->Invalidate (FALSE);
#endif
			//  COMPUTER move
            if (maxc == -1) {
                m_status = end;
            }
                
            if (PlayAt (newx,newy)== FALSE)
			{
                SmartRecalcInfo (lastX,lastY,0); // computer recalc
				cs_state.Lock ();
				m_status = playing;
				cs_state.Unlock ();
            }
			calculatingThread = 0;
	return 0;
}

void SmartRecalcInfo (int x,int y,INFOARR i) 
{
	if (i==0) 
		i=info;
	for (int direction=1;direction<=4;direction++)
	{
		int plusx,plusy;
		switch (direction)
		{
		case 1:plusx=1;plusy=-1;break;
		case 2:plusx=1;plusy=0;break;
		case 3:plusx=1;plusy=1;break;
		case 4:plusx=0;plusy=1;break;
		}
		int srcx=x,srcy=y,c=-1; // left
		do
		{
			c++;
			RecalcInfo (-1,srcx,srcy,direction,i);
			srcx -= plusx;srcy -= plusy;
		} while (InSpace(srcx,srcy)&&(c<=5));

		srcx=x,srcy=y,c=-1; //right
		do
		{
			c++;
			RecalcInfo (-1,srcx,srcy,direction,i);
			srcx += plusx;srcy += plusy;
		} while (InSpace(srcx,srcy)&&(c<=5));
	}
}

void RecalcInfo (int _u,int _x,int _y,int _direction,INFOARR i) 
{
	if (i==0) 
		i=info;

	for (int u = (_u==-1)?1:_u; (_u==-1)?(u<=2):(u<=_u);u++)
		for (int x = (_x==-1)?0:_x;(_x==-1)?(x<=(spacewidth-1)):(x<=_x);x++)
			for (int y = (_y==-1)?0:_y;(_y==-1)?(y<=(spaceheight-1)):(y<=_y);y++)
			{
				if (_direction!=0)
					for (int direction = (_direction==-1)?1:_direction;(_direction==-1)?(direction<= 4):(direction<=_direction);direction++)
						RecalcInfoMain (u,x,y,direction,i);
				i[x][y].ByVal(u)->danger[0]=0;
				if (i[x][y].mark == 0)
					for (int c =1;c<=4;c++)
						i[x][y].ByVal(u)->danger[0]+=i[x][y].ByVal(u)->danger[c]; 
			}
}

void RecalcInfoMain (int u,int x,int y,int direction,INFOARR i) 
{
	int beginx,beginy,plusx,plusy;
	switch (direction)
	{
	case 1:beginx=-1;beginy=1;plusx=1;plusy=-1;break;
	case 2:beginx=-1;beginy=0;plusx=1;plusy=0;break;
	case 3:beginx=-1;beginy=-1;plusx=1;plusy=1;break;
	case 4:beginx=0;beginy=-1;plusx=0;plusy=1;break;
	}
	int l1=-1,l2=-1,r1=-1,r2=-1;
	BOOL l1b=FALSE,l2b=FALSE,r1b=FALSE,r2b=FALSE;
	{// Data load
		int srcx=x,srcy=y;
		do
		{
			srcx -= plusx;srcy -= plusy;
			l1++;
		} while ((InSpace(srcx,srcy)&&(i[srcx][srcy].mark == u)));
		if (!InSpace(srcx,srcy)||(i[srcx][srcy].mark == (u^3)))
			l1b = TRUE;
		do
		{
			srcx -= plusx;srcy -= plusy;
			l2++;
		} while ((InSpace(srcx,srcy))&&(i[srcx][srcy].mark == u));
		if (!InSpace(srcx,srcy)||(i[srcx][srcy].mark == (u^3)))
			l2b = TRUE;
		
		srcx=x; srcy=y;
		do
		{
			srcx += plusx;srcy += plusy;
			r1++;
		} while ((InSpace(srcx,srcy))&&(i[srcx][srcy].mark == u));
		if (!InSpace(srcx,srcy)||(i[srcx][srcy].mark == (u^3)))
			r1b = TRUE;
		do
		{
			srcx += plusx;srcy += plusy;
			r2++;
		} while ((InSpace(srcx,srcy))&&(i[srcx][srcy].mark == u));
		if (!InSpace(srcx,srcy)||(i[srcx][srcy].mark == (u^3)))
			r2b = TRUE;
	}
	//BOOL l1b,l2b,r1b,r2b;
	i[x][y].ByVal(u)->blocks[direction][0] = 0;
	i[x][y].ByVal(u)->blocks[direction][1] = 0;
	i[x][y].ByVal(u)->danger[direction] = 0;
	i[x][y].ByVal(u)->needfor1[direction][0].clear();
	i[x][y].ByVal(u)->needfor2[direction][0].clear();
	i[x][y].ByVal(u)->needfor99[direction][0].clear();
	i[x][y].ByVal(u)->needfor1[direction][1].clear();
	i[x][y].ByVal(u)->needfor2[direction][1].clear();
	i[x][y].ByVal(u)->needfor99[direction][1].clear();
	if ((l1+1+r1) >= 5)
	{
		i[x][y].ByVal(u)->blocks[direction][0] = 0;
		i[x][y].ByVal(u)->blocks[direction][1] = 0;
		i[x][y].ByVal(u)->needfor99[direction][0].push_back (4);
		i[x][y].ByVal(u)->needfor99[direction][1].push_back (0);
		i[x][y].ByVal(u)->needfor99[direction][0].push_back (3);
		i[x][y].ByVal(u)->needfor99[direction][1].push_back (1);
		i[x][y].ByVal(u)->needfor99[direction][0].push_back (2);
		i[x][y].ByVal(u)->needfor99[direction][1].push_back (2);
		i[x][y].ByVal(u)->needfor99[direction][0].push_back (1);
		i[x][y].ByVal(u)->needfor99[direction][1].push_back (3);
		i[x][y].ByVal(u)->needfor99[direction][0].push_back (0);
		i[x][y].ByVal(u)->needfor99[direction][1].push_back (4);
		i[x][y].ByVal(u)->needfor2[direction][0].push_back (0);// TODO
		i[x][y].ByVal(u)->needfor2[direction][1].push_back (0);
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (0);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (0);
		i[x][y].ByVal(u)->danger[direction] = 99;
		return;
	}

	int left=0,right=0;
	{
		if (l1b)
			left = 0;
		else 
		{
			if ((r1+1+l1+1+l2)>=5)
				left = 5;
			else 
			{
				if ((r1+1+l1+1+l2)==4)
				{
					if (l2b||r1b)
						left = 3;
					else left = 4;
				} else left = 3;
			}
		}
		
		if (r1b) 
			right = 0;
		else 
		{
			if ((l1+1+r1+1+r2)>=5)
				right = 5;
			else 
			{
				if ((l1+1+r1+1+r2)==4)
				{
					if (r2b||l1b)
						right = 3;
					else right = 4;
				} else right = 3;
			}
		}
	}

	if ((left==5)&&(right!=5)) // left=5
	{
		i[x][y].ByVal(u)->blocks[direction][0] = l1+1;
		i[x][y].ByVal(u)->blocks[direction][1] = 0;
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (4-r1);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (r1);
		i[x][y].ByVal(u)->danger[direction] = 1;
	}
	if ((left!=5)&&(right==5)) // right =5
	{
		i[x][y].ByVal(u)->blocks[direction][0] = 0;
		i[x][y].ByVal(u)->blocks[direction][1] = r1+1;
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (l1);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (4-l1);
		i[x][y].ByVal(u)->danger[direction] = 1;
	}
	if ((left==5)&&(right==5)) // left=5  && right=5
	{
		i[x][y].ByVal(u)->blocks[direction][0] = l1+1;// block discount 1 danger !!!
		i[x][y].ByVal(u)->blocks[direction][1] = r1+1;
		i[x][y].ByVal(u)->needfor2[direction][0].push_back (4-r1);
		i[x][y].ByVal(u)->needfor2[direction][1].push_back (4-l1);
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (4-r1);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (r1);
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (1l);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (4-l1);
		i[x][y].ByVal(u)->danger[direction] = 2;
	}
	if ((left==4)&&(right!=4)) // left=4
	{
		i[x][y].ByVal(u)->blocks[direction][0] = l1+1+l2+1;
		i[x][y].ByVal(u)->blocks[direction][1] = r1+1;
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (l1+1+l2);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (r1);
		i[x][y].ByVal(u)->danger[direction] = 1;
	}
	if ((left!=4)&&(right==4)) // right =4
	{
		i[x][y].ByVal(u)->blocks[direction][0] = l1+1;
		i[x][y].ByVal(u)->blocks[direction][1] = r1+1+r2+1;
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (l1);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (r1+1+r2);
		i[x][y].ByVal(u)->danger[direction] = 1;
	}
	if ((left==4)&&(right==4)) // left=4  && right=4
	{
		i[x][y].ByVal(u)->blocks[direction][0] = l1+1;
		i[x][y].ByVal(u)->blocks[direction][1] = r1+1;
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (l1+1+l2);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (r1);
		i[x][y].ByVal(u)->needfor1[direction][0].push_back (l1);
		i[x][y].ByVal(u)->needfor1[direction][1].push_back (r1+1+r2);
		i[x][y].ByVal(u)->danger[direction] = 1;
	}
}

BOOL (* LRtoTable (int x,int y,int l,int r,int direction))[spaceheight]
{
	BOOL (*result)[spaceheight]=new BOOL[spacewidth][spaceheight];
	ZeroMemory (result,sizeof (BOOL[spacewidth][spaceheight]));

	int plusx,plusy;
	switch (direction)
	{
	case 1:plusx=1;plusy=-1;break;
	case 2:plusx=1;plusy=0;break;
	case 3:plusx=1;plusy=1;break;
	case 4:plusx=0;plusy=1;break;
	}
	int c;
	for (c =0;c <= l;c++)
		if (InSpace(x-plusx*c,y-plusy*c))
			result[x-plusx*c][y-plusy*c] = TRUE;
	for (c =0;c <= r;c++)
		if (InSpace(x+plusx*c,y+plusy*c))
			result[x+plusx*c][y+plusy*c] = TRUE;

	return result;
}

