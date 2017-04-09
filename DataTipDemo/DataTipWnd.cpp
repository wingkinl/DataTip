// DataTipWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DataTipDemo.h"
#include "DataTipWnd.h"

namespace DataTip {

enum
{
	DTMETRICS_ITEMHEIGHT	= 20,
	DTMETRICS_BUTTONBOUNDS	= 20,
	DTMETRICS_ICONBOUNDS	= 20,
	DTMETRICS_ICONSIZE		= 16,
};

// CDataTipWnd

IMPLEMENT_DYNAMIC(CDataTipWnd, CDataTipWndBase)

CDataTipWnd::CDataTipWnd()
{
	m_dwFlags = FlagsAutoDeleteOnDestroy;
	m_szItem = m_szExpand = CSize(0, 0);
	m_nNameDisplayWidth = -1;
	m_pItemData = nullptr;
	m_pTopItem = nullptr;
	m_pCtrlData = nullptr;
}

CDataTipWnd::~CDataTipWnd()
{
	if (IsRootWindow())
	{
		delete m_pItemData;
		m_pItemData = nullptr;
		delete m_pCtrlData;
		m_pCtrlData = nullptr;
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
	DestroyWindow();
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
	return HandleFromItemData(pItem ? pItem->pFirstChild : nullptr);
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
		pItemData->SetName(lpszName);
	}
	if (nMask & DTIF_VALUE)
	{
		if (!lpszValue)
			return FALSE;
		pItemData->SetValue(lpszValue);
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
	pItemData->SetName(lpszName);
	AdjustLayout(pItemData);
	return TRUE;
}

BOOL CDataTipWnd::SetItemValue(HDTITEM hItem, LPCTSTR lpszValue)
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	pItemData->SetValue(lpszValue);
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
	if (!IsOwnerData())
	{
		ASSERT(0);
		return;
	}
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
	auto pParentItemData = ItemDataFromHandle(hParent);
	if (!pParentItemData || !hInsertAfter)
	{
		ASSERT(0);
		return nullptr;
	}
	if (pParentItemData->nChildren == I_CHILDRENCALLBACK)
	{
		ASSERT(0);
		return nullptr;
	}
	ItemData* pNewItem = new ItemData;
	pNewItem->SetName(lpszName);
	pNewItem->SetValue(lpszValue);
	if (hInsertAfter == DTI_ROOT)
	{
		// Add the item as a root item.
		CDataTipWnd* pRootWnd = GetRootWindow();
		ASSERT(pRootWnd);
		if (pRootWnd->m_pItemData)
		{
			pRootWnd->m_pItemData->pParent = pNewItem;
			pNewItem->pFirstChild = pRootWnd->m_pItemData;
		}
		pRootWnd->m_pItemData = pNewItem;
	}
	else if (hInsertAfter == DTI_FIRST)
	{
		// Inserts the item at the beginning of the list.
		if (pParentItemData->pFirstChild)
		{
			pParentItemData->pFirstChild->pPrev = pNewItem;
		}
		pNewItem->pNext = pParentItemData->pFirstChild;
		pParentItemData->pFirstChild = pNewItem;
	}
	else if (hInsertAfter == DTI_LAST)
	{
		// Inserts the item at the end of the list.
		if (pParentItemData->pFirstChild)
		{
			ItemData* pLastItem = pParentItemData->pFirstChild;
			while (pLastItem->pNext)
			{
				pLastItem = pLastItem->pNext;
			}
			pLastItem->pNext = pNewItem;
			pNewItem->pPrev = pLastItem;
		}
		else
		{
			pParentItemData->pFirstChild = pNewItem;
		}
	}
	else
	{
		auto pInsertAfterItem = ItemDataFromHandle(hInsertAfter);
		if (!pInsertAfterItem)
		{
			ASSERT(0);
			delete pNewItem;
			return nullptr;
		}
		pInsertAfterItem->pNext = pNewItem;
		pNewItem->pPrev = pInsertAfterItem;
	}
	AdjustLayout(pParentItemData);
	return HandleFromItemData(pNewItem);
}

BOOL CDataTipWnd::DeleteItem(HDTITEM hItem)
{
	auto pItem = ItemDataFromHandle(hItem);
	if (!pItem)
	{
		ASSERT(0);
		return FALSE;
	}
	delete pItem;
	return TRUE;
}

BOOL CDataTipWnd::DeleteAllItems()
{
	auto hRoot = GetRootItem();
	if (hRoot)
	{
		delete ItemDataFromHandle(hRoot);
	}
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
	AdjustLayout(pItemData);
	return TRUE;
}

HDTITEM CDataTipWnd::HitTest(CPoint pt, UINT* pFlags) const
{
	CRect rect(0,0,0,0);
	GetClientRect(rect);
	if (!rect.PtInRect(pt))
	{
		if (pFlags)
		{
			*pFlags = 0;
			if (pt.x < rect.left)
				*pFlags |= DTHT_TOLEFT;
			if (pt.x > rect.right)
				*pFlags |= DTHT_TORIGHT;
			if (pt.y < rect.top)
				*pFlags |= DTHT_ABOVE;
			if (pt.y > rect.bottom)
				*pFlags |= DTHT_BELOW;
		}
		return nullptr;
	}
	return HitTestOnClientItem(pt, pFlags);
}

void CDataTipWnd::GetItemsRect(LPRECT lpRect) const
{
	GetClientRect(lpRect);
}

HDTITEM CDataTipWnd::HitTestOnClientItem(CPoint pt, UINT* pFlags) const
{
	CRect rect(0, 0, 0, 0);
	GetItemsRect(rect);
	SHORT nItemHeight = GetItemHeight();
	auto pItemTest = m_pTopItem;
	int nItemCount = 1;
	while (pItemTest && nItemCount <= m_pCtrlData->nMaxDisplayItems)
	{
		LONG nItemBottom = rect.top + nItemHeight;
		if (pt.y >= rect.top && pt.y < nItemBottom)
		{
			if (pFlags)
			{
				LONG left = rect.left;
				if (pt.x < (left += DTMETRICS_BUTTONBOUNDS))
					*pFlags = DTHT_ONITEMBUTTON;
				else if (pt.x < (left += DTMETRICS_ICONBOUNDS))
					*pFlags = DTHT_ONITEMICON;
				else if (pt.x < (left += m_nNameDisplayWidth))
					*pFlags = DTHT_ONITEMNAME;
				else
					*pFlags = DTHT_ONITEMVALUE;
			}
			return HandleFromItemData(pItemTest);
		}
		rect.top = nItemBottom;
		pItemTest = pItemTest->pNext;
		++nItemCount;
	}
	if (pFlags)
		*pFlags = DTHT_NOWHERE;
	return nullptr;
}

BOOL CDataTipWnd::GetItemRect(HDTITEM hItem, LPRECT lpRect, int nPart) const
{
	auto pItemData = ItemDataFromHandle(hItem);
	if (!pItemData)
		return FALSE;
	if (!pItemData->pWnd)
	{
		ASSERT(0);
		return FALSE;
	}
	return pItemData->pWnd->GetItemRectImpl(pItemData, lpRect, nPart);
}

BOOL CDataTipWnd::GetItemRectImpl(ItemData* pItem, LPRECT lpRect, int nPart) const
{
	ASSERT(pItem && pItem->pWnd == this && lpRect);
	GetItemsRect(lpRect);
	SHORT nItemHeight = GetItemHeight();
	auto pItemTest = m_pTopItem;
	int nItemCount = 1;
	while (pItemTest && nItemCount <= m_pCtrlData->nMaxDisplayItems)
	{
		LONG nItemBottom = lpRect->top + nItemHeight;
		if (pItemTest == pItem)
		{
			lpRect->bottom = nItemBottom;
			switch (nPart)
			{
			case DTP_BUTTON:
				lpRect->right = lpRect->left + DTMETRICS_BUTTONBOUNDS;
				break;
			case DTP_IMAGE:
				lpRect->left += DTMETRICS_BUTTONBOUNDS;
				lpRect->right = lpRect->left + DTMETRICS_ICONBOUNDS;
				break;
			case DTP_NAME:
				lpRect->left += DTMETRICS_BUTTONBOUNDS + DTMETRICS_ICONBOUNDS;
				lpRect->right = lpRect->left + m_nNameDisplayWidth;
				break;
			case DTP_VALUE:
				lpRect->left += DTMETRICS_BUTTONBOUNDS + DTMETRICS_ICONBOUNDS + m_nNameDisplayWidth;
				break;
			case DTP_BOUNDS:
				break;
			}
			return TRUE;
		}
		lpRect->top = nItemBottom;
		pItemTest = pItemTest->pNext;
		++nItemCount;
	}
	return FALSE;
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