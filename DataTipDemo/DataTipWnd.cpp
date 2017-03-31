// DataTipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DataTipDemo.h"
#include "DataTipWnd.h"


// CDataTipWnd

IMPLEMENT_DYNAMIC(CDataTipWnd, CDataTipWndBase)

CDataTipWnd::CDataTipWnd()
{
	m_bAutoDeleteOnDestroy = TRUE;
}

CDataTipWnd::~CDataTipWnd()
{
}


BOOL CDataTipWnd::Create(CWnd* pOwner, DWORD dwStyle)
{
	dwStyle |= WS_CLIPSIBLINGS;
	CString strClassName = ::AfxRegisterWndClass(CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE + 1), NULL);
	DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE;
	CRect rect(0,0,0,0);
	BOOL bRet = CreateEx(dwExStyle, (LPCTSTR)strClassName, _T(""), dwStyle, rect, nullptr, 0);
	return bRet;
}

void CDataTipWnd::Show(int nX, int nY)
{
	UINT nFlags = SWP_NOSIZE|SWP_NOACTIVATE;
	SetWindowPos(&wndTopMost, nX, nY, -1, -1, nFlags);
	ShowWindow(SW_SHOWNOACTIVATE);
}

void CDataTipWnd::Show(const CPoint& pt)
{
	Show(pt.x, pt.y);
}

void CDataTipWnd::PostNcDestroy()
{
	CDataTipWndBase::PostNcDestroy();
	if (m_bAutoDeleteOnDestroy)
		delete this;
}

BEGIN_MESSAGE_MAP(CDataTipWnd, CDataTipWndBase)
END_MESSAGE_MAP()



// CDataTipWnd message handlers


