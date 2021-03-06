
// DataTipDemoDlg.h : header file
//

#pragma once

namespace DataTip{
class CDataTipWnd;
}

// CDataTipDemoDlg dialog
class CDataTipDemoDlg : public CDialogEx
{
// Construction
public:
	CDataTipDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATATIPDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	DataTip::CDataTipWnd* m_pDataTip;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
