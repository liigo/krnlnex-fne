
#include "cmem.h"

///////////////////////////////////

static MEM_BUF _m_membuf;  // !!! 不允许直接引用此变量。

///////////////////////////////////

static void CMem_init_ (CMem* pThis)
{
	pThis->m_pMemBuf = &_m_membuf;
	pThis->m_pData = NULL;
	pThis->m_nAllocedSize = pThis->m_nSize = 0;
}

static void _CMem_Free (CMem* pThis, LPBYTE pData)
{
	if (pData == NULL)  return;

	if (pData < pThis->m_pMemBuf->m_buf ||
            pData >= pThis->m_pMemBuf->m_buf + PAGE_BUF_SIZE)
	{
		free (pData);
	}
	else
	{
		pThis->m_pMemBuf->m_btaryUsed [(pData - pThis->m_pMemBuf->m_buf) / PAGE_SIZE] = 0;
		pThis->m_pMemBuf->m_nPageUsedCount--;
	}
}

static LPBYTE CMem_GetPtr (CMem* pThis)
{
	return pThis->m_nSize == 0 ? NULL : pThis->m_pData;
}

static int CMem_GetSize (CMem* pThis)
{
	return pThis->m_nSize;
}

static BOOL CMem_IsEmpty (CMem* pThis)
{
	return pThis->m_nSize == 0;
}

static void CMem_Empty (CMem* pThis)
{
    pThis->m_nSize = 0;
}

static BOOL CMem_CopyFrom (CMem* pThis, LPBYTE pData, int nSize)
{
	CMem_Empty (pThis);
	return CMem_Append (pThis, pData, nSize);
}

static LPBYTE CMem_Alloc (CMem* pThis, int nSize)
{
    int n;

	if (pThis->m_pData != NULL)
		return CMem_Realloc (pThis, nSize);

	if (nSize == 0)  return NULL;

	if (nSize <= PAGE_SIZE && pThis->m_pMemBuf->m_nPageUsedCount < PAGE_COUNT)
	{
		n = FindByte (pThis->m_pMemBuf->m_btaryUsed, PAGE_COUNT, 0);

		pThis->m_pMemBuf->m_btaryUsed [n] = 1;
		pThis->m_pMemBuf->m_nPageUsedCount++;

		pThis->m_nAllocedSize = PAGE_SIZE;
		pThis->m_pData = pThis->m_pMemBuf->m_buf + n * PAGE_SIZE;
		pThis->m_nSize = nSize;
		return pThis->m_pData;
	}

	pThis->m_pData = (LPBYTE)malloc (nSize + FM_GROW_SIZE);
	if (pThis->m_pData == NULL)
	{
		pThis->m_nAllocedSize = 0;
		pThis->m_nSize = 0;
	}
	else
	{
		pThis->m_nAllocedSize = nSize + FM_GROW_SIZE;
		pThis->m_nSize = nSize;
	}
	return pThis->m_pData;
}

static LPBYTE CMem_Realloc (CMem* pThis, int nSize)
{
    BOOL blIsAllocBuf;
	LPBYTE pNew;

	if (nSize == 0)
	{
		CMem_Free (pThis);
		return NULL;
	}
	if (pThis->m_pData == NULL)
		return CMem_Alloc (pThis, nSize);

	blIsAllocBuf = pThis->m_pData >= pThis->m_pMemBuf->m_buf &&
			pThis->m_pData < pThis->m_pMemBuf->m_buf + PAGE_BUF_SIZE;

	if (pThis->m_nAllocedSize >= nSize && (blIsAllocBuf == TRUE ||
			pThis->m_nAllocedSize - nSize <= FM_MAX_FREE_BUF_SIZE + FM_GROW_SIZE))
	{
		pThis->m_nSize = nSize;
		return pThis->m_pData;
	}

	if (blIsAllocBuf == TRUE)
	{
		pNew = (LPBYTE)malloc (nSize + FM_GROW_SIZE);
		if (pNew != NULL)
		{
			memcpy (pNew, pThis->m_pData, min (nSize, pThis->m_nSize));
			_CMem_Free (pThis, pThis->m_pData);
			pThis->m_pData = pNew;
			pThis->m_nAllocedSize = nSize + FM_GROW_SIZE;
			pThis->m_nSize = nSize;
			return pThis->m_pData;
		}
		else
			return NULL;
	}

	pNew = (LPBYTE)realloc (pThis->m_pData, nSize + FM_GROW_SIZE);
	if (pNew != NULL)
	{
		pThis->m_pData = pNew;
		pThis->m_nAllocedSize = nSize + FM_GROW_SIZE;
		pThis->m_nSize = nSize;
		return pThis->m_pData;
	}
	else
		return NULL;
}

static void CMem_Free (CMem* pThis)
{
	_CMem_Free (pThis, pThis->m_pData);

	pThis->m_pData = NULL;
	pThis->m_nAllocedSize = pThis->m_nSize = 0;
}

static void CMem_AddDouble (CMem* pThis, double db)
{
    CMem_Append (pThis, (LPBYTE)&db, sizeof (db));
}

static void CMem_AddFloat (CMem* pThis, float flt)
{
    CMem_Append (pThis, (LPBYTE)&flt, sizeof (flt));
}

static void CMem_AddLong (CMem* pThis, long lVal)
{
    CMem_Append (pThis, (LPBYTE)&lVal, sizeof (lVal));
}

static void CMem_AddDWord (CMem* pThis, DWORD dw)
{
    CMem_Append (pThis, (LPBYTE)&dw, sizeof (dw));
}

static void CMem_AddInt (CMem* pThis, int n)
{
    CMem_Append (pThis, (LPBYTE)&n, sizeof (n));
}

static void CMem_AddWord (CMem* pThis, WORD w)
{
    CMem_Append (pThis, (LPBYTE)&w, sizeof (w));
}

static void CMem_AddShort (CMem* pThis, short sht)
{
    CMem_Append (pThis, (LPBYTE)&sht, sizeof (sht));
}

static void CMem_AddByte (CMem* pThis, BYTE bt)
{
    CMem_Append (pThis, &bt, sizeof (bt));
}

static void CMem_AddStr (CMem* pThis, char* pstr)
{
    CMem_AddText (pThis, pstr);
	CMem_Append (pThis, NULL, 1);
}

static void CMem_AddText (CMem* pThis, char* pstr)
{
	if (pstr != NULL && *pstr != '\0')
		CMem_Append (pThis, (LPBYTE)pstr, strlen (pstr));
}

static void CMem_XchgMem (CMem* pThis, CMem* pMem)
{
    int n;
    LPBYTE p;
    PMEM_BUF pMemBuf;

	pMemBuf = pThis->m_pMemBuf;
	pThis->m_pMemBuf = pMem->m_pMemBuf;
	pMem->m_pMemBuf = pMemBuf;

	p = pThis->m_pData;
	pThis->m_pData = pMem->m_pData;
	pMem->m_pData = p;

	n = pThis->m_nAllocedSize;
	pThis->m_nAllocedSize = pMem->m_nAllocedSize;
	pMem->m_nAllocedSize = n;

	n = pThis->m_nSize;
	pThis->m_nSize = pMem->m_nSize;
	pMem->m_nSize = n;
}

static int CMem_ReadFile (CMem* pThis, char* szFileName, int nReadLength)
{
	int nResult, n;
    LPBYTE p;
    FILE* in;

    nResult = -1;
	in = fopen (szFileName, "rb");
	if (in != NULL)
	{
		if (nReadLength == -1)
		{
			fseek (in, 0, SEEK_END);
			nReadLength = (INT)ftell (in);
			fseek (in, 0, SEEK_SET);
		}

		if (nReadLength > 0)
		{
			p = CMem_Alloc (pThis, nReadLength);
			if (p != NULL)
			{
				n = fread (p, 1, nReadLength, in);
				if (!ferror (in))
				{
					if (n != nReadLength)
						CMem_Realloc (pThis, n);
					fclose (in);
					return n;
				}
			}
		}
		else
			nResult = 0;

		fclose (in);
	}

	CMem_Free (pThis);
	return nResult;
}

static BOOL CMem_WriteFile (CMem* pThis, char* szFileName, int nWriteLength)
{
    int nSize;
    FILE* out;

	out = fopen (szFileName, "wb");
	if (out != NULL)
	{
		nSize = CMem_GetSize (pThis);
		if (nWriteLength == -1)
			nWriteLength = nSize;
		else
			nWriteLength = min (nWriteLength, nSize);

		if (nWriteLength == 0 || (INT)fwrite (CMem_GetPtr (pThis),
                1, nWriteLength, out) == nWriteLength)
		{
			fclose (out);
			return TRUE;
		}

		fclose (out);
	}

	return FALSE;
}

// if pData == NULL, insert blank data.
static BOOL CMem_Insert (CMem* pThis, int nOffset, LPBYTE pData, int nSize)
{
    int nOldSize;
    LPBYTE pMem, pInsert;

	if (nSize == 0)  return TRUE;
	if (nOffset == pThis->m_nSize)
        return CMem_Append (pThis, pData, nSize);

	nOldSize = pThis->m_nSize;
	pMem = CMem_Realloc (pThis, max (nOffset, nOldSize) + nSize);
	if (pMem == NULL)  return FALSE;

	pInsert = pMem + nOffset;
	if (nOffset < nOldSize)
		memmove (pInsert + nSize, pInsert, nOldSize - nOffset);

	if (pData != NULL)
		memmove (pInsert, pData, nSize);
	else
		memset (pInsert, 0, nSize);

	return TRUE;
}

// if pData == NULL, append blank data.
static BOOL CMem_Append (CMem* pThis, LPBYTE pData, int nSize)
{
    int nOldSize;
    LPBYTE pMem, pAppend;

	if (nSize <= 0)  return TRUE;

	nOldSize = pThis->m_nSize;
	pMem = CMem_Realloc (pThis, nOldSize + nSize);
	if (pMem == NULL)  return FALSE;

	pAppend = pMem + nOldSize;
	if (pData != NULL)
		memmove (pAppend, pData, nSize);
	else
		memset (pAppend, 0, nSize);

	return TRUE;
}

// if pReplaceData == NULL, replace blank data.
static BOOL CMem_Replace (CMem* pThis, int nOffset, int nSize,
                       LPBYTE pReplaceData, int nReplaceSize)
{
	LPBYTE pReplaceAt;
    int nOldSize, nMoveSize;

	nOldSize = pThis->m_nSize;
	if (nSize == 0 || nOffset >= nOldSize)
		return CMem_Insert (pThis, nOffset, pReplaceData, nReplaceSize);
	if (nReplaceSize == 0)  
		return CMem_Remove (pThis, nOffset, nSize);

	if (nSize > nOldSize - nOffset)
        nSize = nOldSize - nOffset;

	nMoveSize = nOldSize - nOffset - nSize;
	if (nReplaceSize > nSize)
	{
		pReplaceAt = CMem_Realloc (pThis, nOldSize + nReplaceSize - nSize);
		if (pReplaceAt == NULL)  return FALSE;
		pReplaceAt += nOffset;

		if (nMoveSize > 0)
			memmove (pReplaceAt + nReplaceSize, pReplaceAt + nSize, nMoveSize);
	}
	else
	{
		pReplaceAt = pThis->m_pData;
		pReplaceAt += nOffset;

		if (nReplaceSize < nSize)
		{
			if (nMoveSize > 0)
				memmove (pReplaceAt + nReplaceSize, pReplaceAt + nSize, nMoveSize);

			pReplaceAt = CMem_Realloc (pThis, nOldSize - (nSize - nReplaceSize));
			if (pReplaceAt == NULL)  return FALSE;  // data already be changed!
			pReplaceAt += nOffset;
		}
	}

	if (pReplaceData != NULL)
		memmove (pReplaceAt, pReplaceData, nReplaceSize);
	else
		memset (pReplaceAt, 0, nReplaceSize);
	return TRUE;
}

// if nOffset == -1, delete data from tail.
static BOOL CMem_Remove (CMem* pThis, int nOffset, int nSize)
{
    int nNewSize, nRemoveSize;
    LPBYTE pRemoveAt;

	if (nOffset == -1)
	{
		nNewSize = max (0, pThis->m_nSize - nSize);
		if (nNewSize == 0)
		{
			CMem_Free (pThis);
			return TRUE;
		}
		else
			return CMem_Realloc (pThis, nNewSize) != NULL;
	}

	if (nSize == 0)  return TRUE;
	if (pThis->m_pData == NULL || nOffset >= pThis->m_nSize)
        return FALSE;
	
	pRemoveAt = pThis->m_pData + nOffset;
	
	if (nSize < pThis->m_nSize - nOffset)
	{
		nRemoveSize = nSize;
		memmove (pRemoveAt, pRemoveAt + nRemoveSize,
                pThis->m_nSize - nOffset - nRemoveSize);
	}
	else
		nRemoveSize = pThis->m_nSize - nOffset;

	if (pThis->m_nSize == nRemoveSize)
	{
		CMem_Free (pThis);
		return TRUE;
	}
	else
		return CMem_Realloc (pThis, pThis->m_nSize - nRemoveSize) != NULL;
}

//////////////////////////////////////////////////////////

static void CString_init_str_ (CString* pThis, CString* pStrSrc)
{
    CString_init_ (pThis);

	CMem_CopyFrom ((CMem*)pThis, CMem_GetPtr ((CMem*)pStrSrc),
            CMem_GetSize ((CMem*)pStrSrc));
}

static void CString_init_text_ (CString* pThis, char* ps)
{
    CString_init_ (pThis);

	if (ps != NULL && *ps != '\0')
		CMem_AddStr ((CMem*)pThis, ps);
}

static void CString_Delete (CString* pThis, int nIndex, int nCount)
{
    int nLen = CString_GetLength (pThis);

    if (nIndex < nLen)
    {
        CMem_Remove ((CMem*)pThis, nIndex, min (nLen - nIndex, nCount));
    }
}

static void CString_Insert (CString* pThis, int nIndex, char* szText)
{
    if (nIndex <= CString_GetLength (pThis))
        CMem_Insert ((CMem*)pThis, nIndex, (LPBYTE)szText, strlen (szText));
}

static char* CString_GetText (CString* pThis)
{
	return CMem_GetSize ((CMem*)pThis) == 0 ? "" :
            (char*)CMem_GetPtr ((CMem*)pThis);
}

static int CString_GetLength (CString* pThis)
{
	return CMem_GetSize ((CMem*)pThis) == 0 ? 0 :
            strlen ((char*)CMem_GetPtr ((CMem*)pThis));
}

static BOOL CString_IsEmpty (CString* pThis)
{
	return CString_GetLength (pThis) == 0;
}

static void CString_Empty (CString* pThis)
{
	CMem_Free ((CMem*)pThis);
}

static void CString_SetStr (CString* pThis, CString* pStrSrc)
{
	CMem_CopyFrom ((CMem*)pThis, CMem_GetPtr ((CMem*)pStrSrc),
            CMem_GetSize ((CMem*)pStrSrc));
}

static void CString_SetChar (CString* pThis, char ch)
{
    char buf [2];

	CMem_Free ((CMem*)pThis);
	if (ch != '\0')
    {
        buf [0] = ch;
        buf [1] = '\0';
        CMem_Append ((CMem*)pThis, (LPBYTE)buf, sizeof (buf));
    }
}

static void CString_SetText (CString* pThis, char* ps)
{
	CMem_Free ((CMem*)pThis);
	if (ps != NULL && *ps != '\0')
		CMem_AddStr ((CMem*)pThis, ps);
}

static void CString_SetTextPart (CString* pThis, char* ps, int nLength)
{
	CMem_Free ((CMem*)pThis);
	if (ps != NULL && *ps != '\0')
    {
        CMem_Append ((CMem*)pThis, (LPBYTE)ps, min (nLength, strlen (ps)));
        CMem_AddByte ((CMem*)pThis, 0);
    }
}

static void CString_AddStr (CString* pThis, CString* pStrSrc)
{
	if (CString_GetLength (pStrSrc) > 0)
	{
		CMem_Realloc ((CMem*)pThis, CString_GetLength (pThis));
		CMem_Append ((CMem*)pThis, CMem_GetPtr ((CMem*)pStrSrc),
                CMem_GetSize ((CMem*)pStrSrc));
	}
}

static void CString_AddChar (CString* pThis, char ch)
{
    char buf [2];

	if (ch != '\0')
	{
		CMem_Realloc ((CMem*)pThis, CString_GetLength (pThis));
        buf [0] = ch;
        buf [1] = '\0';
        CMem_Append ((CMem*)pThis, (LPBYTE)buf, sizeof (buf));
	}
}

static void CString_AddText (CString* pThis, char* ps)
{
	if (ps != NULL && *ps != '\0')
	{
		CMem_Realloc ((CMem*)pThis, CString_GetLength (pThis));
		CMem_Append ((CMem*)pThis, (LPBYTE)ps, strlen (ps) + 1);
	}
}

static void CString_GetLeft (CString* pThis, int nCount, CString* pDest)
{
    CMem mem;
    CMem_init_ (&mem);

	nCount = min (nCount, CString_GetLength (pThis));

    CMem_Append (&mem, CMem_GetPtr ((CMem*)pThis), nCount);
    CMem_Empty (pDest);

	if (nCount > 0)
	{
		CMem_Append ((CMem*)pDest, CMem_GetPtr (&mem), nCount);
		CMem_AddByte ((CMem*)pDest, 0);
	}

    CMem_destroy_ (&mem);
}

static void CString_GetRight (CString* pThis, int nCount, CString* pDest)
{
    int nLen;
    CMem mem;
    CMem_init_ (&mem);

    nLen = CString_GetLength (pThis);

    CMem_Append (&mem, CMem_GetPtr ((CMem*)pThis), CMem_GetSize ((CMem*)pThis));
    CMem_Empty (pDest);

	nCount = min (nCount, nLen);
	if (nCount > 0)
    {
		CMem_Append ((CMem*)pDest, CMem_GetPtr (&mem) + nLen - nCount, nCount + 1);
    }

    CMem_destroy_ (&mem);
}

static void CString_MakeUpper (CString* pThis)
{
	LPBYTE p;
    int i, nLen;

    p = CMem_GetPtr ((CMem*)pThis);
	nLen = CString_GetLength (pThis);
	for (i = 0; i < nLen; p++, i++)
	{
		if (*p & 0x80)
		{
			p++;
			i++;
		}
		else if (*p >= 'a' && *p <= 'z')
			*p = *p - 'a' + 'A';
	}
}

static void CString_MakeLower (CString* pThis)
{
	LPBYTE p;
    int i, nLen;

	p = CMem_GetPtr ((CMem*)pThis);
	nLen = CString_GetLength (pThis);
	for (i = 0; i < nLen; p++, i++)
	{
		if (*p & 0x80)
		{
			p++;
			i++;
		}
		else if (*p >= 'A' && *p <= 'Z')
			*p = *p - 'A' + 'a';
	}
}

static void CString_TrimRight (CString* pThis)
{
	LPBYTE p;
    int i, nLen;

	p = CMem_GetPtr ((CMem*)pThis);
	nLen = CString_GetLength (pThis);
	for (i = nLen - 1; i >= 0; i--)
	{
		if (p [i] != ' ')
			break;
	}

	if (i + 1 != nLen)
	{
		CMem_Realloc ((CMem*)pThis, i + 1);
		CMem_AddByte ((CMem*)pThis, 0);
	}
}

static void CString_TrimLeft (CString* pThis)
{
	LPBYTE p;
    int i, nLen;

	p = CMem_GetPtr ((CMem*)pThis);
	nLen = CString_GetLength (pThis);

	for (i = 0; i < nLen; i++)
	{
		if (p [i] != ' ')
			break;
	}

	if (i > 0)
	{
		nLen = nLen - i + 1;
		memmove (p, p + i, nLen);
		CMem_Realloc ((CMem*)pThis, nLen);
	}
}

static BOOL CString_EndOfText (CString* pThis, char* ps)
{
    int nLen, nStrLen;

	nLen = CString_GetLength (pThis);
	nStrLen = strlen (ps);
	return nStrLen == 0 || nLen >= nStrLen &&
			strcmp ((char*)(CMem_GetPtr ((CMem*)pThis) + nLen - nStrLen), ps) == 0;
}

static BOOL CString_EndOfChar (CString* pThis, char ch)
{
	return CString_GetLength (pThis) > 0 && *(CMem_GetPtr ((CMem*)pThis) +
            CMem_GetSize ((CMem*)pThis) - 1) == ch;
}

//////////////////////////////////////////////////////////

static int CDWordArray_GetSize (CDWordArray* pThis)
{
    return CMem_GetSize ((CMem*)pThis) / sizeof (DWORD);
}

static BOOL CDWordArray_SetSize (CDWordArray* pThis, int nNewSize)
{
    return CMem_Realloc ((CMem*)pThis, nNewSize * sizeof (DWORD)) != NULL;
}

static int CDWordArray_GetUpperBound (CDWordArray* pThis)
{
	return CDWordArray_GetSize (pThis) - 1;
}

static void CDWordArray_RemoveAll (CDWordArray* pThis)
{
	CMem_Free ((CMem*)pThis);
}

static LPDWORD CDWordArray_GetPtr (CDWordArray* pThis)
{
	return (LPDWORD)CMem_GetPtr ((CMem*)pThis);
}

static DWORD CDWordArray_GetAt (CDWordArray* pThis, int nIndex)
{
    // ASSERT (nIndex >= 0 && nIndex < CDWordArray_GetSize (pThis));
	return CDWordArray_GetPtr (pThis) [nIndex];
}

static void CDWordArray_SetAt (CDWordArray* pThis, int nIndex, DWORD dwElement)
{
    // ASSERT (nIndex >= 0 && nIndex < CDWordArray_GetSize (pThis));
	CDWordArray_GetPtr (pThis) [nIndex] = dwElement;
}

static int CDWordArray_Append (CDWordArray* pThis, CDWordArray* pSrc)
{
    int n;

	n = CDWordArray_GetSize (pThis);
	CMem_Append ((CMem*)pThis, CMem_GetPtr ((CMem*)pSrc),
            CMem_GetSize ((CMem*)pSrc));
	return n;
}

static void CDWordArray_Copy (CDWordArray* pThis, CDWordArray* pSrc)
{
    CMem_Empty ((CMem*)pThis);
	CMem_Append ((CMem*)pThis, CMem_GetPtr ((CMem*)pSrc),
            CMem_GetSize ((CMem*)pSrc));
}

static void CDWordArray_Add (CDWordArray* pThis, DWORD dwElement)
{
    CMem_AddDWord ((CMem*)pThis, dwElement);
}

static void CDWordArray_InsertAt (CDWordArray* pThis, int nIndex, DWORD dwElement)
{
	CMem_Insert ((CMem*)pThis, nIndex * sizeof (DWORD),
            (LPBYTE)&dwElement, sizeof (DWORD));
}

static void CDWordArray_RemoveAt (CDWordArray* pThis, int nIndex, int nCount)
{
	CMem_Remove ((CMem*)pThis, nIndex * sizeof (DWORD),
            sizeof (DWORD) * nCount);
}

static INT CDWordArray_Find (CDWordArray* pThis, DWORD dwFind)
{
    INT i, nCount;

    nCount = CDWordArray_GetSize (pThis);
    for (i = 0; i < nCount; i++)
    {
        if (CDWordArray_GetAt (pThis, i) == dwFind)
            return i;
    }

    return -1;
}

