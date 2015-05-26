#ifndef _SHARE_HPP
#define _SHARE_HPP

#include "elib_h\elib.h"


#define LIB_SUPPORT_OS		(_LIB_OS  (__OS_WIN))
#define DT_SUPPORT_OS		(_DT_OS   (__OS_WIN))
#define CMD_SUPPORT_OS		(_CMD_OS  (__OS_WIN))
#define PROP_SUPPORT_OS		(_PROP_OS (__OS_WIN))
#define EVENT_SUPPORT_OS	(_EVENT_OS(__OS_WIN))


///////////////////////////////////////////////////////////////////////////////
// 通用函数
//
// GetIntByIndex()/SetIntByIndex()/GetPointerByIndex(): 
// 取/置数据类型相关索引处的成员值,已测试, 索引值>=0

#define _private  /*标记为私有*/

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif
//-----------------------------------------------------------------------------




//!!! 以下函数均假设数据类型之成员全部为INT类型(或同样占用4个字节)
//index: 0..

_private void* _GetPointerByIndex(void *pBase, int index)
{
	return (void*) ((INT)pBase + index * sizeof(INT));
}

_private INT _GetIntByIndex(void *pBase, INT index)
{
	return *((INT*)_GetPointerByIndex(pBase,index));
}

_private void _SetIntByIndex(void *pBase, INT index, INT value)
{
	*((INT*)_GetPointerByIndex(pBase,index)) = value;
}


INT GetIntByIndex(PMDATA_INF pArgInf, INT index)
{
	return _GetIntByIndex(pArgInf->m_pCompoundData,index);
}

void SetIntByIndex(PMDATA_INF pArgInf, INT index, INT value)
{
	_SetIntByIndex(pArgInf->m_pCompoundData,index,value);
}

void* GetPointerByIndex(PMDATA_INF pArgInf, INT index)
{
	return _GetPointerByIndex(pArgInf->m_pCompoundData,index);
}

//存取全局数值
#define GlobalDataCount 	1024	//如果不够可增加此数值
_private DWORD g_GlobalDatas[GlobalDataCount];
void ClearGlobalDatas(void)
{
	memset(g_GlobalDatas, NULL, sizeof(DWORD)*GlobalDataCount);
}
void SetGlobalData(int index, void* p)
{
	if(index<0 || index>=GlobalDataCount) return;
	g_GlobalDatas[index] = (DWORD)p;
}
void* GetGlobalData(int index)
{
	if(index<0 || index>=GlobalDataCount) return NULL;
	return (void*)g_GlobalDatas[index];
}

typedef enum
{
	fi_OutputDebugInfo=0, fi_Assert, fi_TimeToText, fi_dir,
}
FuncIndex;

typedef struct 
{
	char* pFuncName;
	FuncIndex iFuncIndex;
}FuncNameIndex;

static FuncNameIndex g_FuncsNameIndex[] =
{
	{"输出调试文本", fi_OutputDebugInfo},
	{"检查", fi_Assert},
	{"时间到文本", fi_TimeToText},
	/*{"寻找文件", fi_dir},*/ //此函数没有执行代码!!!???
};

void SetKrnlnFuncAddr(FuncIndex index, void* pFuncAddr)
{
	SetGlobalData((int)index, pFuncAddr);
}
PFN_EXECUTE_CMD GetKrnlnFuncAddr(FuncIndex index)
{
	return (PFN_EXECUTE_CMD) GetGlobalData((int)index);
}

//调用核心库中的命令
BOOL CallKrnlnFunc(FuncIndex index, PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)
{
	PFN_EXECUTE_CMD pFunc = GetKrnlnFuncAddr(index);
	if(pFunc)
	{
		pFunc(pRetData, nArgCount, pArgInf);
		return TRUE;
	}
	return FALSE;
}

//从易核心库中取所有需要的函数地址
#define FreeLibraryAddReturn() {::FreeLibrary(hModule); return;}
void Get_Krnln_Functions_Address()
{
	HMODULE hModule = ::LoadLibrary("krnln.fne");
	if(hModule==NULL)
	{
		hModule = ::LoadLibrary("krnln.fnr");
		if(hModule == NULL) return;
	}

	try{
	//-------------------------------------------------------------------------
	PFN_GET_LIB_INFO pfn_GetNewInfo = 
		(PFN_GET_LIB_INFO)::GetProcAddress(hModule, "GetNewInf");
	if(pfn_GetNewInfo==NULL) FreeLibraryAddReturn();
	
	PLIB_INFO pLibInfo = pfn_GetNewInfo();
	if(pLibInfo==NULL) FreeLibraryAddReturn();

	int nCmdCount = pLibInfo->m_nCmdCount;
	PCMD_INFO pCmdsInfo = pLibInfo->m_pBeginCmdInfo;
	PFN_EXECUTE_CMD* pCmdsFun = pLibInfo->m_pCmdsFunc;

	int nNeededFuncCount = sizeof(g_FuncsNameIndex) / sizeof(g_FuncsNameIndex[0]);
	for(int i=0; i<nCmdCount; i++)
	{
		PCMD_INFO pCmdInfo = &(pCmdsInfo[i]);
		if(pCmdInfo==NULL) continue;
		if(pCmdInfo->m_szName==NULL) continue;
		if(strlen(pCmdInfo->m_szName)<=0) continue;
		if(pCmdInfo->m_wState & CT_IS_HIDED == 0) continue;
		//TRACE("%s \n", pCmdInfo->m_szName);

		for(int j=0; j<nNeededFuncCount; j++)
		{
			if(g_FuncsNameIndex[j].pFuncName == NULL) continue;
			if(strcmp(pCmdInfo->m_szName, g_FuncsNameIndex[j].pFuncName)==0)
			{
				//ASSERT(pCmdsFun[i]); //确实有一些命令的执行地址为NULL, 如"寻找文件" (其它如:控制语句,算术运算,数组操作等).
				SetKrnlnFuncAddr(g_FuncsNameIndex[j].iFuncIndex, (void*)pCmdsFun[i]);
				break;
			}
		}
	}
	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
	FreeLibraryAddReturn();
}





//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif


class CFreqMem
{
private:
	unsigned char* m_pdata;
	size_t m_datalen, m_bufferlen;
public:
	CFreqMem()
	{
		m_pdata = NULL; m_datalen = m_bufferlen = 0;
	}
	void* GetPtr()
	{
		return (m_datalen == 0 ? NULL : m_pdata);
	}
	void AddDWord(DWORD dw)
	{
		AppendData(&dw, sizeof(dw));
	}
	void AppendData(void* pdata, size_t size)
	{
		if(m_bufferlen - m_datalen < size)
		{
			if(m_pdata == NULL)
			{
				m_bufferlen = 128;
				m_pdata = (unsigned char*) malloc(m_bufferlen);
				//assert(m_datalen == 0);
			}
			while(m_bufferlen - m_datalen < size)
			{
				m_bufferlen *= 2;
			};
			m_pdata = (unsigned char*) realloc(m_pdata, m_bufferlen);
		}
		memcpy(m_pdata + m_datalen, pdata, size);
		m_datalen += size;
	}
};



#endif //_SHARE_HPP