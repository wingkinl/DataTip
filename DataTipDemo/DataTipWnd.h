#pragma once

// CDataTipWnd

typedef CWnd	CDataTipWndBase;

class CDataTipWnd : public CDataTipWndBase
{
	DECLARE_DYNAMIC(CDataTipWnd)
public:
	CDataTipWnd();
	virtual ~CDataTipWnd();
public:
	BOOL Create(CWnd* pOwner, DWORD dwStyle = 0);

	void Show(const CPoint& pt);

	void Show(int nX, int nY);

	void Pop();
protected:
	
protected:
	void PostNcDestroy() override;
public:
	BOOL	m_bAutoDeleteOnDestroy;
protected:
	CWnd*	m_pOwner;
protected:

	DECLARE_MESSAGE_MAP()
};


