#pragma once

namespace DataTip{

struct _DTITEM;
typedef struct _DTITEM *HDTITEM;

enum DTItemMask
{
	DTIF_NAME       = 0x00000001,
	DTIF_VALUE      = 0x00000002,
	DTIF_IMAGE      = 0x00000004,
	DTIF_PARAM      = 0x00000008,
	DTIF_HANDLE     = 0x00000010,
	DTIF_CHILDREN   = 0x00000020,
};

struct DTITEM 
{
	UINT         mask;
	HDTITEM      hItem;
	LPTSTR       pszName;
	LPTSTR       pszValue;
	int          cchTextMax;
	int          iImage;
	int          cChildren;
	LPARAM       lParam;
};

struct NMDTDISPINFO
{
	NMHDR	hdr;
	DTITEM	item;
};

enum DTWndStyle
{
	DTS_OWNERDATA	= 0x0001,
	DTS_SINGLEWND	= 0x0002,	// display as single window instead of cascaded windows
};

enum DTGetNextItem
{
	DTGN_ROOT,
	DTGN_PARENT,
	DTGN_CHILD,
	DTGN_NEXT,
	DTGN_PREVIOUS,
};

#define DTI_ROOT                ((HDTITEM)(ULONG_PTR)-0x10000)
#define DTI_FIRST               ((HDTITEM)(ULONG_PTR)-0x0FFFF)
#define DTI_LAST                ((HDTITEM)(ULONG_PTR)-0x0FFFE)

enum DTHitTest
{
	DTHT_NOWHERE            = 0x0001,
	DTHT_ONITEMICON         = 0x0002,
	DTHT_ONITEMNAME         = 0x0004,
	DTHT_ONITEMVALUE        = 0x0008,
	DTHT_ONITEMBUTTON       = 0x0010,
	DTHT_ONITEMLABEL        = (DTHT_ONITEMNAME | DTHT_ONITEMVALUE),
	DTHT_ONITEM             = (DTHT_ONITEMBUTTON | DTHT_ONITEMICON | DTHT_ONITEMLABEL),

	DTHT_ABOVE              = 0x0100,
	DTHT_BELOW              = 0x0200,
	DTHT_TORIGHT            = 0x0400,
	DTHT_TOLEFT             = 0x0800,
};

enum DTPart
{
	DTP_BUTTON,
	DTP_IMAGE,
	DTP_NAME,
	DTP_VALUE,
	DTP_BOUNDS,
};

enum DTExpand
{
	DTE_COLLAPSE,
	DTE_EXPAND,
	DTE_TOGGLE,
};

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

	// Retrieves the next item having the given relationship with the
	// specified item.
	HDTITEM GetNextItem(HDTITEM hItem, DTGetNextItem nCode) const;

	// Retrieves the next child item after the specified item.
	HDTITEM GetChildItem(HDTITEM hItem) const;

	// Retrieves the next sibling item after the specified item.
	HDTITEM GetNextSiblingItem(HDTITEM hItem) const;

	// Retrieves the previous sibling item of the specified item.
	HDTITEM GetPrevSiblingItem(HDTITEM hItem) const;

	// Retrieves the parent of the specified item.
	HDTITEM GetParentItem(HDTITEM hItem) const;

	// Retrieves the root item of the control, NULL if none.
	HDTITEM GetRootItem() const;

	// Retrieves information about a particular item in the control.
	BOOL GetItem(DTITEM* pItem) const;

	// Retrieves the text associated with the given item.
	CString GetItemName(HDTITEM hItem) const;

	CString GetItemValue(HDTITEM hItem) const;

	// Retrieves the images associated with the given item.
	BOOL GetItemImage(HDTITEM hItem, int& nImage) const;

	// Retrieves the user-supplied data associated with the given item.
	DWORD_PTR GetItemData(HDTITEM hItem) const;

	// Sets the state of the an item.
	BOOL SetItem(DTITEM* pItem);
	BOOL SetItem(HDTITEM hItem, UINT nMask, LPCTSTR lpszName, LPCTSTR lpszValue,
		int nImage, LPARAM lParam);

	// Sets the text of the specified item.
	BOOL SetItemName(HDTITEM hItem, LPCTSTR lpszName);

	BOOL SetItemValue(HDTITEM hItem, LPCTSTR lpszValue);

	// Sets the image on the specified item.
	BOOL SetItemImage(HDTITEM hItem, int nImage);

	// Sets the user data on the specified item.
	BOOL SetItemData(HDTITEM hItem, DWORD_PTR dwData);

	// Determines if the specified item has children.
	int GetItemChildrenCount(HDTITEM hItem) const;

	void SetItemChildrenCount(HDTITEM hItem, int nCount);

	// Retrieves the background colour used throughout the control.
	COLORREF GetBkColor() const;

	// Sets the background color to be used throughout the control.
	COLORREF SetBkColor(COLORREF clr);

	// Retrieves the height of items in the control.
	SHORT GetItemHeight() const;

	// Sets the height of items in the control.
	SHORT SetItemHeight(SHORT cyHeight);

	// Retrieves the text color used for all items in the control.
	COLORREF GetTextColor() const;

	// Sets the text color used for all items in the control.
	COLORREF SetTextColor(COLORREF clr);

	// Inserts a new item to the control.
	HDTITEM InsertItem(LPCTSTR lpszName, LPCTSTR lpszValue, 
		HDTITEM hParent = DTI_ROOT, HDTITEM hInsertAfter = DTI_LAST);

	// Removes the specified item from the control.
	BOOL DeleteItem(HDTITEM hItem);

	// Removes all items from the control.
	BOOL DeleteAllItems();

	// Expands the children of the specified item.
	BOOL Expand(HDTITEM hItem, DTExpand exp);

	// Determines the visual feature of the control under
	// the specified point.
	virtual HDTITEM HitTest(CPoint pt, UINT* pFlags = NULL) const;

	// Retrieves the bounding rectangle for a specified part of a specified item
	BOOL GetItemRect(HDTITEM hItem, LPRECT lpRect, int nPart) const;

	CImageList* SetImageList(CImageList* pImageList);
protected:
	void PostNcDestroy() override;
	
	BOOL IsOwnerData() const;
	BOOL IsDisplayAsSingleWindow() const;

	CSize GetDisplaySize() const;

	inline CDataTipWnd* GetRootWindow() const
	{
		ASSERT(m_pCtrlData);
		return m_pCtrlData->pRootWnd;
	}

	inline BOOL IsRootWindow() const
	{
		return GetRootWindow() == this;
	}

	virtual void AdjustLayout();
	
	void Redraw();

	virtual HDTITEM HitTestOnClientItem(CPoint pt, UINT* pFlags) const;

	virtual void GetItemsRect(LPRECT lpRect) const;
protected:
	enum
	{
		FlagsAutoDeleteOnDestroy	= 0x00000001,
		FlagsOwnsItemData			= 0x00000002,
		FlagsExpand					= 0x00000004,
	};
	DWORD		m_dwFlags;
	// for display
	CSize		m_szItem;		// unexpanded
	CSize		m_szExpand;		// contains children
	int			m_nNameDisplayWidth;	// auto-expand during scrolling
	// for use when the window does not own the item data
	int			m_nFirstVisibleIndex;
	
	// data of individual item
	struct ItemData 
	{
		enum Flags
		{
			FlagsNameCallback		= 0x00010000,
			FlagsValueCallback		= 0x00020000,
			FlagsExpand				= 0x00000001,
		};
		DWORD			dwFlags;
		CString			strName;
		CString			strValue;
		int				nImage;
		int				nChildren;
		LPARAM			lParam;
		ItemData*		pParent;
		ItemData*		pFirstChild;// the first child
		ItemData*		pNext;		// next sibling
		ItemData*		pPrev;		// previous sibling
		CDataTipWnd*	pWnd;

		ItemData()
		{
			dwFlags = 0;
			nImage = I_IMAGENONE;
			nChildren = 0;
			lParam = 0;
			pParent = pFirstChild = pNext = pPrev = nullptr;
			pWnd = nullptr;
		}
		~ItemData()
		{
			if (pWnd)
			{
				pWnd->m_pItemData = nullptr;
				pWnd->DestroyWindow();
				pWnd = nullptr;
			}
			if (pPrev)
				pPrev->pNext = pNext;
			if (pNext)
				pNext->pPrev = pPrev;
			if (pParent)
			{
				--pParent->nChildren;
				ASSERT(pParent->nChildren >= 0);
				if (pParent->pFirstChild == this)
					pParent->pFirstChild = pNext;
			}
			if (pFirstChild)
			{
				ItemData* pNextChild = pFirstChild;
				while (pNextChild)
				{
					ItemData* pDelete = pNextChild;
					pNextChild = pNextChild->pNext;
					delete pDelete;
				}
			}
		}
		
		void SetName(LPCTSTR pszName)
		{
			if (pszName == LPSTR_TEXTCALLBACK)
			{
				dwFlags |= FlagsNameCallback;
				strName.Empty();
			}
			else
			{
				dwFlags &= ~FlagsNameCallback;
				strName = pszName;
			}
		}
		void SetValue(LPCTSTR pszName)
		{
			if (pszName == LPSTR_TEXTCALLBACK)
			{
				dwFlags |= FlagsValueCallback;
				strValue.Empty();
			}
			else
			{
				dwFlags &= ~FlagsValueCallback;
				strValue = pszName;
			}
		}
	};
	ItemData*	m_pItemData;	// the first item, not necessarily the first visible one
	ItemData*	m_pTopItem;		// the first visible item

	// data of the whole control, shared across all items
	struct CtrlData 
	{
		CDataTipWnd*	pRootWnd;
		typedef CArray<CDataTipWnd*>	DataTipWnds;
		DataTipWnds		arrPopupWnds;
		CImageList*		pImageList;
		SHORT			nItemHeight;
		int				nMaxDisplayItems;
		int				nMaxValueDisplayWidth;
		COLORREF		clrBack;
		COLORREF		clrText;
		CSize			szIcon;
	};
	CtrlData*	m_pCtrlData;
protected:
	inline ItemData*	ItemDataFromHandle(HDTITEM hItem) const
	{
		if (hItem == DTI_ROOT)
			hItem = GetRootItem();
		return reinterpret_cast<ItemData*>(hItem);
	}
	inline static HDTITEM HandleFromItemData(ItemData* pItem)
	{
		return (HDTITEM)pItem;
	}
	inline static void AdjustLayout(ItemData* pItemData)
	{
		if (pItemData && pItemData->pWnd)
			pItemData->pWnd->AdjustLayout();
	}

	BOOL GetItemRectImpl(ItemData* pItem, LPRECT lpRect, int nPart) const;
protected:

	DECLARE_MESSAGE_MAP()
};

}
