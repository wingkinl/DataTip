// DataTipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DataTipDemo.h"
#include "DataTipWnd.h"

namespace DataTip {

// CDataTipWnd

IMPLEMENT_DYNAMIC(CDataTipWnd, CDataTipWndBase)

CDataTipWnd::CDataTipWnd()
{
	m_dwFlags = FlagsAutoDeleteOnDestroy;
	m_szItem = m_szExpand = CSize(0, 0);
	m_nFirstVisibleChild = 0;
	m_pItemData = nullptr;
	m_pCtrlData = nullptr;
}

CDataTipWnd::~CDataTipWnd()
{
	if (IsRootWindow())
	{
		// TODO
		delete m_pItemData;
		delete m_pCtrlData;
	}
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

void CDataTipWnd::Pop()
{
	ASSERT(m_pCtrlData);
	for (INT_PTR ii = m_pCtrlData->arrPopupWnds.GetSize()-1; ii >= 0; --ii)
	{
		m_pCtrlData->arrPopupWnds[ii]->DestroyWindow();
	}
	GetRootWindow()->DestroyWindow();
}

HDTITEM CDataTipWnd::GetNextItem(HDTITEM hItem, DTGetNextItem nCode) const
{
	switch (nCode)
	{
	case DTGN_ROOT:
		return GetRootItem();
	case DTGN_PARENT:
		return GetParentItem(hItem);
	case DTGN_CHILD:
		return GetChildItem(hItem);
	case DTGN_NEXT:
		return GetNextSiblingItem(hItem);
	case DTGN_PREVIOUS:
		return GetPrevSiblingItem(hItem);
	}
	return nullptr;
}

HDTITEM CDataTipWnd::GetChildItem(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	return HandleFromItemData(pItem ? pItem->pChild : nullptr);
}

HDTITEM CDataTipWnd::GetNextSiblingItem(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	return HandleFromItemData(pItem ? pItem->pNext : nullptr);
}

HDTITEM CDataTipWnd::GetPrevSiblingItem(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	return HandleFromItemData(pItem ? pItem->pPrev : nullptr);
}

HDTITEM CDataTipWnd::GetParentItem(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	return HandleFromItemData(pItem ? pItem->pParent : nullptr);
}

HDTITEM CDataTipWnd::GetRootItem() const
{
	auto pRootWnd = GetRootWindow();
	return HandleFromItemData(pRootWnd ? pRootWnd->m_pItemData : nullptr);
}

BOOL CDataTipWnd::GetItem(DTITEM* pItem) const
{
	if (!pItem)
		return FALSE;
	ItemData* pItemData = ItemDataFromHandle(pItem->hItem);
	if (!pItemData)
		return FALSE;
	if (pItem->mask & DTIF_NAME)
	{
		if (!pItem->pszName)
			return FALSE;
		_tcsncpy(pItem->pszName, (LPCTSTR)pItemData->strName, pItem->cchTextMax);
	}
	if (pItem->mask & DTIF_VALUE)
	{
		if (!pItem->pszValue)
			return FALSE;
		_tcsncpy(pItem->pszValue, (LPCTSTR)pItemData->strValue, pItem->cchTextMax);
	}
	if (pItem->mask & DTIF_IMAGE)
		pItem->iImage = pItemData->nImage;
	if (pItem->mask & DTIF_PARAM)
		pItem->lParam = pItemData->lParam;
	if (pItem->mask & DTIF_CHILDREN)
		pItem->cChildren = pItemData->nChildren;
	return TRUE;
}

CString CDataTipWnd::GetItemName(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	if (!pItem)
		return CString();
	return pItem->strName;
}

CString CDataTipWnd::GetItemValue(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	if (!pItem)
		return CString();
	return pItem->strValue;
}

BOOL CDataTipWnd::GetItemImage(HDTITEM hItem, int& nImage) const
{
	auto pItem = ItemDataFromHandle(hItem);
	if (!pItem)
		return I_IMAGENONE;
	return pItem->nImage;
}

DWORD_PTR CDataTipWnd::GetItemData(HDTITEM hItem) const
{
	auto pItem = ItemDataFromHandle(hItem);
	if (!pItem)
		return 0;
	return pItem->lParam;
}

BOOL CDataTipWnd::SetItem(DTITEM* pItem)
{
	if (!pItem)
		return FALSE;
	return SetItem(pItem->hItem, pItem->mask, 
		pItem->pszName, pItem->pszValue, pItem->iImage, pItem->lParam);
}

BOOL CDataTipWnd::SetItem(HDTITEM hItem, UINT nMask, LPCTSTR lpszName, LPCTSTR lpszValue, 
	int nImage, LPARAM lParam)
{
	ItemData* pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	if (nMask & DTIF_NAME)
	{
		if (!lpszName)
			return FALSE;
		pItemData->strName = lpszName;
	}
	if (nMask & DTIF_VALUE)
	{
		if (!lpszValue)
			return FALSE;
		pItemData->strValue = lpszValue;
	}
	if (nMask & DTIF_IMAGE)
		pItemData->nImage = nImage;
	if (nMask & DTIF_PARAM)
		pItemData->lParam = lParam;
	AdjustLayout(pItemData);
	return TRUE;
}

BOOL CDataTipWnd::SetItemName(HDTITEM hItem, LPCTSTR lpszName)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	pItemData->strName = lpszName;
	AdjustLayout(pItemData);
	return TRUE;
}

BOOL CDataTipWnd::SetItemValue(HDTITEM hItem, LPCTSTR lpszValue)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	pItemData->strValue = lpszValue;
	AdjustLayout(pItemData);
	return TRUE;
}

BOOL CDataTipWnd::SetItemImage(HDTITEM hItem, int nImage)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	pItemData->nImage = nImage;
	AdjustLayout(pItemData);
	return TRUE;
}

BOOL CDataTipWnd::SetItemData(HDTITEM hItem, DWORD_PTR dwData)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	pItemData->lParam = dwData;
	AdjustLayout(pItemData);
	return TRUE;
}

int CDataTipWnd::GetItemChildrenCount(HDTITEM hItem) const
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return 0;
	return pItemData->nChildren;
}

void CDataTipWnd::SetItemChildrenCount(HDTITEM hItem, int nCount)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return;
	pItemData->nChildren = nCount;
	AdjustLayout(pItemData);
}

COLORREF CDataTipWnd::GetBkColor() const
{
	ASSERT(m_pCtrlData);
	return m_pCtrlData->clrBack;
}

COLORREF CDataTipWnd::SetBkColor(COLORREF clr)
{
	ASSERT(m_pCtrlData);
	COLORREF clrOld = m_pCtrlData->clrBack;
	m_pCtrlData->clrBack = clr;
	Redraw();
	return clrOld;
}

SHORT CDataTipWnd::GetItemHeight() const
{
	ASSERT(m_pCtrlData);
	return m_pCtrlData->nItemHeight;
}

SHORT CDataTipWnd::SetItemHeight(SHORT cyHeight)
{
	ASSERT(m_pCtrlData);
	SHORT nOldHeight = m_pCtrlData->nItemHeight;
	m_pCtrlData->nItemHeight = cyHeight;
	AdjustLayout();
	return nOldHeight;
}

COLORREF CDataTipWnd::GetTextColor() const
{
	ASSERT(m_pCtrlData);
	return m_pCtrlData->clrText;
}

COLORREF CDataTipWnd::SetTextColor(COLORREF clr)
{
	ASSERT(m_pCtrlData);
	COLORREF clrOld = m_pCtrlData->clrText;
	m_pCtrlData->clrText = clr;
	Redraw();
	return clrOld;
}

HDTITEM CDataTipWnd::InsertItem(LPCTSTR lpszName, LPCTSTR lpszValue, 
	HDTITEM hParent, HDTITEM hInsertAfter)
{
	// TODO
	return nullptr;
}

BOOL CDataTipWnd::DeleteItem(HDTITEM hItem)
{
	// TODO
	return TRUE;
}

BOOL CDataTipWnd::DeleteAllItems()
{
	// TODO
	return TRUE;
}

BOOL CDataTipWnd::Expand(HDTITEM hItem, DTExpand exp)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	BOOL bWasExpanded = (pItemData->dwFlags & ItemData::FlagsExpand) != 0;
	BOOL bExpand = exp == DTE_TOGGLE ? (!bWasExpanded) : (exp == DTE_EXPAND);
	if (bExpand)
	{
		pItemData->dwFlags |= ItemData::FlagsExpand;
		if (pItemData->pWnd)
			pItemData->pWnd->m_dwFlags |= FlagsExpand;
	}
	else
	{
		pItemData->dwFlags &= ~ItemData::FlagsExpand;
		if (pItemData->pWnd)
			pItemData->pWnd->m_dwFlags &= ~FlagsExpand;
	}
	// TODO
	return TRUE;
}

HDTITEM CDataTipWnd::HitTest(CPoint pt, DTHitTest* pArea) const
{
	// TODO
	return nullptr;
}

CImageList* CDataTipWnd::SetImageList(CImageList* pImageList)
{
	ASSERT(m_pCtrlData);
	CImageList* pOldImageList = m_pCtrlData->pImageList;
	m_pCtrlData->pImageList = pImageList;
	return pOldImageList;
}

void CDataTipWnd::PostNcDestroy()
{
	CDataTipWndBase::PostNcDestroy();
	if (m_dwFlags & FlagsAutoDeleteOnDestroy)
		delete this;
}

BOOL CDataTipWnd::IsOwnerData() const
{
	return (GetStyle() & DTS_OWNERDATA) != 0;
}

BOOL CDataTipWnd::IsDisplayAsSingleWindow() const
{
	return (GetStyle() & DTS_SINGLEWND) != 0;
}

CSize CDataTipWnd::GetDisplaySize() const
{
	ASSERT(m_pItemData);
	if (m_dwFlags & FlagsExpand)
		return m_szExpand;
	return m_szItem;
}

void CDataTipWnd::AdjustLayout()
{
	Redraw();
}

void CDataTipWnd::Redraw()
{
	RedrawWindow(NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME);
}

BEGIN_MESSAGE_MAP(CDataTipWnd, CDataTipWndBase)
END_MESSAGE_MAP()



// CDataTipWnd message handlers


}