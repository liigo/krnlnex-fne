#ifndef GLOBAL_FUNCTION_HPP
#define GLOBAL_FUNCTION_HPP

#include "share.hpp"
#include <math.h>

#ifndef __E_STATIC_LIB

///////////////////////////////////////////////////////////////////////////////
// 组件方法
#define Global_All_functions \
	KRNLNEX_CMD (fn_Global_Trace),\
	KRNLNEX_CMD (fn_Global_Verify),\
	KRNLNEX_CMD (fn_Global_EnumerateFiles),\
	KRNLNEX_CMD (fn_Global_EnumerateDirs),\
	KRNLNEX_CMD (fn_Global_CallFunction),\
	KRNLNEX_CMD (fn_Global_Wait),\
	KRNLNEX_CMD (fn_Global_DelayCallFunction),\
	KRNLNEX_CMD (fn_Global_GetVarAddr),\
	KRNLNEX_NULL_CMD_2,\
	KRNLNEX_NULL_CMD_10,\


///////////////////////////////////////////////////////////////////////////////
// 组件命令信息

#define Global_All_CmdInfo \
	Global_Trace_CmdInfo,\
	Global_Verify_CmdInfo,\
	Global_EnumerateFiles_CmdInfo,\
	Global_EnumerateDirs_CmdInfo,\
	Global_CallFunction_CmdInfo,\
	Global_Wait_CmdInfo,\
	Global_DelayCallFunction_CmdInfo,\
	Global_GetVarAddr_CmdInfo,\
	MASK_CMD_INFO_2,\
	MASK_CMD_INFO_10,\

#endif


///////////////////////////////////////////////////////////////////////////////
// 全局命令之定义与实现 (同一个方法的参数、信息及实现代码放在一起，便于查找及参考)

//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_Trace_ArgInfo[] =
{
	{
/*name*/	_WT("欲输出值"),
/*explain*/	_WT("对于非“文本型”参数，将自动转换为文本。本参数可接受任意基本类型，但不接受数组以及自定义数据类型。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	AS_DEFAULT_VALUE_IS_EMPTY|AS_RECEIVE_ALL_TYPE_DATA,
	},
};

#define Global_Trace_CmdInfo \
	{\
/*ccname*/	_WT("调试输出"),\
/*egname*/	_WT("Trace"),\
/*explain*/	_WT("类似于易语言核心库中的“输出调试文本()”命令，向输出面板输出一行调试文本。"\
	"本命令可接受任意多个通用型(任意基本类型)参数及数组。"\
	"与“输出调试文本()”不同，本命令的所有参数数据仅输出为一行文本（各参数之间以“ | ”间隔）"\
	"——既保持了数据相关性，又有效利用了输出面板的横向宽度。"\
	"根据易友迪克的提议，改善了字节集的输出格式，形如“字节集:n{13,10}”，其中n为字节集长度。"\
	"另外，本命令对各种数据的输出格式都颇为友好，如数组被输出为形如“数组:3,2{...}”，其中“3,2”表示该数组为3行2列，{}之间为实际数组数据。"),\
/*category*/17,\
/*state*/	CT_IS_HIDED|CMD_SUPPORT_OS|CT_ALLOW_APPEND_NEW_ARG|CT_DISABLED_IN_RELEASE,\
/*ret*/		_SDT_NULL,\
/*reserved*/0,\
/*level*/	LVL_SIMPLE,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_Trace_ArgInfo) / sizeof(Global_Trace_ArgInfo[0]),\
/*arg lp*/	Global_Trace_ArgInfo,\
	}

#endif

void Trace_DoWithParam(XString* pStr, PMDATA_INF pParam)
{
	switch(pParam->m_dtDataType) 
	{
	case SDT_INT:
		XString_AddTextF(pStr, "%d | ", pParam->m_int); break;
	case SDT_SHORT: 
		XString_AddTextF(pStr, "%d | ", (short)pParam->m_int); break;
	case SDT_BYTE:
		 XString_AddTextF(pStr, "%d | ", (byte)pParam->m_int); break;
	case SDT_INT64:
		XString_AddTextF(pStr, "%I64d | ", pParam->m_int64); break;
	case SDT_FLOAT: 
		XString_AddTextF(pStr, "%f | ", pParam->m_float); break;
	case SDT_DOUBLE:
		XString_AddTextF(pStr, "%f | ", pParam->m_double); break;
	case SDT_BOOL:
		XString_AddTextF(pStr, "%s | ", pParam->m_bool?"真":"假"); break;
	case SDT_TEXT: //前后加半角双引号
	{
		char* pText = pParam->m_pText;
		if(pText==NULL || strlen(pText)<=0)
		{
			XString_AddText(pStr, "“” | "); 
		}
		else
		{
			//XString_AddTextF(pStr, "文本:%d", strlen(pText));
			XString_AddText(pStr, "“");
			XString_AddText(pStr, pText);
			XString_AddText(pStr, "” | ");
		}
		break;
	}
	case SDT_BIN:
	{
		if(pParam->m_pBin == NULL) //! 字节集为空的情况一定要处理，否则指针访问越界!
		{
			XString_AddText(pStr, "字节集:0{} | ");
			break;
		}
		int bytes = _GetIntByIndex(pParam->m_pBin, 1);
		LPBYTE pBin = (LPBYTE)_GetPointerByIndex(pParam->m_pBin, 2);
		//根据易友迪克的提议，改善了对字节集的输出格式，形如“字节集:2{13,10}”
		XString_AddTextF(pStr, "字节集:%d", bytes);
		XString_AddText(pStr, "{");
		for(int i=0; i<bytes; i++)
		{
			XString_AddTextF(pStr, i==bytes-1?"%d":"%d,", (BYTE)*(pBin+i));
			/*
			if(i > 50) //防止输出文本太长，输出面板显示不全!
			{	
				XString_AddText(pStr, "..."); break;
			}
			*/
		}
			
		XString_AddText(pStr, "} | ");
		//XString_AddText(pStr, "(bin) | "); break;
		break;
	}
	case SDT_DATE_TIME:
	{	
		//调用“日期到文本()”进行转换
		MDATA_INF RetData, ArgInf[2];
		ArgInf[0].m_dtDataType = SDT_DATE_TIME;
		ArgInf[0].m_date = pParam->m_date;
		ArgInf[1].m_dtDataType = SDT_INT;
		ArgInf[1].m_int = 1; //1、#全部转换； 2、#日期部分； 3、#时间部分。
		CallKrnlnFunc(fi_TimeToText, &RetData, 1, ArgInf);
		XString_AddText(pStr, "[");
		XString_AddText(pStr, RetData.m_pText);
		XString_AddText(pStr, "] | ");
		MFree(RetData.m_pText); //!
		break;
	}
	case SDT_SUB_PTR:
		XString_AddTextF(pStr, "(子程序地址:%d) | ", pParam->m_dwSubCodeAdr); break;
	case _SDT_NULL:
		XString_AddText(pStr, "(空) | "); break;
	default: XString_AddText(pStr, "(?未知?) | "); break;
	}
}
void Trace_DoWithParam_Array(XString* pStr, PMDATA_INF pParam)
{
	int nDimension = _GetIntByIndex(pParam->m_pAryData, 0); //维数
	int nElementCount = 1;
	int i=0;
	XString_AddText(pStr, "数组:");
	
	for(i=0; i<nDimension; i++)
	{
		XString_AddTextF(pStr, "%s%d", i==0 ? "" : "*",
						_GetIntByIndex(pParam->m_pAryData, i+1));
		nElementCount *= _GetIntByIndex(pParam->m_pAryData, i+1);
	}
	XString_AddText(pStr, "{");
	
	//p指向数组数据起始区
	LPBYTE p = (LPBYTE)_GetPointerByIndex(pParam->m_pAryData, nDimension+1);
	char s[5]; strcpy(s, ",");
	for(i=0; i<nElementCount; i++)
	{
		if(i == nElementCount-1) strcpy(s, "");
		switch(pParam->m_dtDataType) 
		{
		case SDT_INT: 
			XString_AddTextF(pStr, "%d%s", *(INT*)p,s); p+=sizeof(INT); break;
		case SDT_SHORT: 
			XString_AddTextF(pStr, "%d%s", *(SHORT*)p,s); p+=sizeof(SHORT); break;
		case SDT_BYTE:
			XString_AddTextF(pStr, "%d%s", *(BYTE*)p,s); p+=sizeof(BYTE); break;
		case SDT_INT64:
			XString_AddTextF(pStr, "%I64d%s", *(INT64*)p,s); p+=sizeof(INT64); break;
		case SDT_FLOAT: 
			XString_AddTextF(pStr, "%f%s", *(FLOAT*)p,s); p+=sizeof(FLOAT); break;
		case SDT_DOUBLE:
			XString_AddTextF(pStr, "%f%s", *(DOUBLE*)p,s); p+=sizeof(DOUBLE); break;
		case SDT_BOOL:
			XString_AddTextF(pStr, "%s%s", (*(BOOL*)p)==TRUE?"真":"假",s); p+=sizeof(BOOL); break;
		case SDT_TEXT:
		{
			char* pText = (char*) (*(DWORD*)p);
			p += sizeof(DWORD);
			XString_AddText(pStr, "“");
			if(pText!=NULL && strlen(pText)>0)
			{
				XString_AddText(pStr, pText);
			}
			XString_AddText(pStr, "”");
			XString_AddTextF(pStr, "%s", s);
			break;
		}
		case SDT_BIN:
		{
			if((*(DWORD*)p) == 0) //! 字节集为空的情况一定要处理，否则指针访问越界!
			{
				XString_AddTextF(pStr, "字节集:0{}%s", s);
				p += sizeof(DWORD);
				break;
			}
			int bytes = _GetIntByIndex((void*)(*(DWORD*)p), 1);
			LPBYTE pBin = (LPBYTE)_GetPointerByIndex((void*)(*(DWORD*)p), 2);
			p += sizeof(DWORD);
			//根据易友迪克的提议，改善了对字节集的输出格式，形如“字节集:2字节{13,10}”
			XString_AddTextF(pStr, "字节集:%d", bytes);
			XString_AddText(pStr, "{");
			for(int j=0; j<bytes; j++)
			{
				XString_AddTextF(pStr, j==bytes-1?"%d":"%d,", (BYTE)*(pBin+j));
			}
			XString_AddTextF(pStr, "}%s", s);
			break;
		}
		case SDT_DATE_TIME: 
		{	
			//调用“日期到文本()”进行转换
			MDATA_INF RetData, ArgInf[2];
			ArgInf[0].m_dtDataType = SDT_DATE_TIME;
			ArgInf[0].m_date = *(DATE*)p;  p+=sizeof(DATE);
			ArgInf[1].m_dtDataType = SDT_INT;
			ArgInf[1].m_int = 1; //1、#全部转换； 2、#日期部分； 3、#时间部分。
			CallKrnlnFunc(fi_TimeToText, &RetData, 1, ArgInf);
			XString_AddText(pStr, "[");
			XString_AddText(pStr, RetData.m_pText);
			XString_AddText(pStr, "]");
			XString_AddTextF(pStr, "%s", s);
			MFree(RetData.m_pText); //!
			break;
		}
		case SDT_SUB_PTR:
			XString_AddTextF(pStr, "(子程序地址:%d)%s", *(DWORD*)p,s);  p+=sizeof(DWORD); break;
		case _SDT_NULL:
			XString_AddTextF(pStr, "(空)%s", s); p++; break; //p++ 防止死循环!
		default: XString_AddTextF(pStr, "(?未知?)%s", s); p++; break;
		}
	}
	XString_AddText(pStr, "} | ");
}
DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_Trace)
{
	try{
	//-------------------------------------------------------------------------
	XString str; XString_init_(&str);

	for(int i=0; i<nArgCount; i++)
	{
		PMDATA_INF pParam = &pArgInf[i];

		BOOL bIsArray = ((pParam->m_dtDataType & DT_IS_ARY) !=0 );
		if(bIsArray) pParam->m_dtDataType &= (~DT_IS_ARY);

		if(!bIsArray) //参数为非数组情况
		{
			Trace_DoWithParam(&str, pParam);
		}
		else //参数为数组的情况
		{
			Trace_DoWithParam_Array(&str, pParam);
		}
	}

	char* pStr = XString_GetText(&str); 
	if(pStr == NULL)
	{
		XString_destroy_(&str); return;
	}
	int chars=strlen(pStr);
	if(chars >= 3) pStr[chars-3] = '\0'; //删除最后的“ | ”

	//转调“输出调试文本()”
	MDATA_INF RetData, ArgInf;
	ArgInf.m_dtDataType = SDT_TEXT;
	ArgInf.m_pText = pStr;
	CallKrnlnFunc(fi_OutputDebugInfo, &RetData, 1, &ArgInf);

	XString_destroy_(&str);
	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); /*Beep(500, 100);*/ return; }
}


//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_Verify_ArgInfo[] =
{
	{
/*name*/	_WT("欲检查值"),
/*explain*/	_WT("可以为普通数值、变量、命令/方法调用、子语句等。如果本参数为命令/方法调用或子语句，总是会被执行，即使在非调试版本中。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	AS_RECEIVE_ALL_TYPE_DATA|AS_DEFAULT_VALUE_IS_EMPTY,
	},
};

#define Global_Verify_CmdInfo \
	{\
/*ccname*/	_WT("验证"),\
/*egname*/	_WT("Verify"),\
/*explain*/	_WT("类似于易语言核心库中的“检查()”命令，本命令在调试版中验证参数值(或其计算结果)是否为“零值”，"\
	"如果是“零值”则易程序的执行将被暂停且警示（相当于设置了条件断点）。"\
	"与“检查()”命令不同的是，本命令在运行版中仅仅忽略“检查”这个步骤，但不会放弃执行参数语句（这在参数为命令/方法调用或子语句的情况下尤为重要）；"\
	"此外，本命令可以检查任意基本类型的参数，且可以同时检查多个参数。"\
	"如果提供了多个参数，所有的参数都会被验证，但只要出现一个“零值”，程序就会被暂停且警示。"\
	"“零值”定义：字节型/短整数型/整数型/长整数型值等于0，小数型/双精度小数型绝对值小于0.00001，"\
	"逻辑型值为“假”，文本型/字节集型数据长度等于0，数组成员数等于0，日期时间值为“1899/12/30 00:00:00”，"\
	"子程序指针的执行地址为0。注：目前暂不支持“子语句类型”的参数。"),\
/*category*/17,\
/*state*/	CT_IS_HIDED|CMD_SUPPORT_OS|CT_ALLOW_APPEND_NEW_ARG,\
/*ret*/		_SDT_NULL,\
/*reserved*/0,\
/*level*/	LVL_SIMPLE,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_Verify_ArgInfo) / sizeof(Global_Verify_ArgInfo[0]),\
/*arg lp*/	Global_Verify_ArgInfo,\
	}

#endif

BOOL fn_verify_IsZeroValue(PMDATA_INF pParam)
{
	BOOL bIsZeroValue = FALSE;
	
	BOOL bIsArray = ((pParam->m_dtDataType & DT_IS_ARY) !=0 );
	if(bIsArray) pParam->m_dtDataType &= (~DT_IS_ARY);
	
	if(bIsArray)
	{
		int nArrayElementCount = 0;
		LPBYTE p = GetAryElementInf(pParam->m_pAryData, &nArrayElementCount);
		bIsZeroValue = (p==NULL || nArrayElementCount==0);
	}
	else
	{
		switch(pParam->m_dtDataType) 
		{
		case SDT_INT: case SDT_SHORT: case SDT_BYTE:
			bIsZeroValue = pParam->m_int == 0; break;
		case SDT_INT64:
			bIsZeroValue = pParam->m_int64 ==0; break;
		case SDT_FLOAT: 
			bIsZeroValue = (fabs(pParam->m_float)  < 0.00001); break;
		case SDT_DOUBLE:
			bIsZeroValue = (fabs(pParam->m_double) < 0.00001); break;
		case SDT_BOOL:
			bIsZeroValue = (pParam->m_bool==FALSE); break;
		case SDT_TEXT:
		{
			char* pText = pParam->m_pText;
			bIsZeroValue = (pText==NULL || strlen(pText)<=0);
			break;
		}
		case SDT_BIN:
		{
			if(pParam->m_pBin == NULL) //! 字节集为空的情况!
			{
				bIsZeroValue = TRUE; break;
			}
			int bytes = _GetIntByIndex(pParam->m_pBin, 1);
			LPBYTE pBin = (LPBYTE)_GetPointerByIndex(pParam->m_pBin, 2);
			bIsZeroValue = (pBin==NULL || bytes==0);
			break;
		}
		case SDT_DATE_TIME: //小于0也有意义, 但易语言初始值为0: 1899-12-30
			bIsZeroValue = (fabs((DOUBLE)pParam->m_date) < 0.00001); break;
		case SDT_SUB_PTR:
			bIsZeroValue = ((DWORD)pParam->m_dwSubCodeAdr == 0); break;
		case _SDT_NULL:
			bIsZeroValue = FALSE; break; //允许省略参数，不警示
		case SDT_STATMENT:
			//todo: 执行子语句并取其值
			bIsZeroValue = FALSE; //!!!
			break;
		default: bIsZeroValue = FALSE; break;
		}
	} //end of if(bIsArray)-else

	return bIsZeroValue;
}
DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_Verify)
{
	try{
	//-------------------------------------------------------------------------
	BOOL bVerifyFailed = FALSE;

	for(int i=0; i<nArgCount; i++)
	{
		if(fn_verify_IsZeroValue(&pArgInf[i])) //对所有的参数进行验证
			bVerifyFailed = TRUE;
	}
	
	//如果验证失败且为调试版则转调“检查()”
	if(bVerifyFailed && NotifySys(NRS_GET_PRG_TYPE, 0, 0)==PT_DEBUG_RUN_VER)
	{
		MDATA_INF RetData, ArgInf;
		ArgInf.m_dtDataType = SDT_BOOL;
		ArgInf.m_bool = FALSE; //!
		CallKrnlnFunc(fi_Assert, &RetData, 1, &ArgInf);
	}
	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}


//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_EnumerateFiles_ArgInfo[] =
{
	{
/*name*/	_WT("要搜索的目录"),
/*explain*/	_WT("目录文本的最后可以有也可以没有“\\”字符。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_TEXT,
/*default*/	0,
/*state*/	NULL,
	},
	{
/*name*/	_WT("是否搜索子目录"),
/*explain*/	_WT("如果为“真”，则搜索指定目录及其中的所有子目录（递归）；否则只搜索指定目录。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_BOOL,
/*default*/	1,
/*state*/	AS_HAS_DEFAULT_VALUE,
	},
	{
/*name*/	_WT("保留参数"),
/*explain*/	_WT(""),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_INT,
/*default*/	0,
/*state*/	AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
/*name*/	_WT("用户处理子程序"),
/*explain*/	_WT("用户提供的子程序必须符合以下要求：1、具有“逻辑型”返回值；2、有且只有一个“文本型”参数（用于接收文件名称）。"\
	"在一次枚举过程中，该子程序会被多次调用（每枚举到一个文件就调用一次）。"\
	"用户可根据参数传入的“全路径文件名称”对该文件进行所需操作（读写删除等不限）。"\
	"如果在此子程序中返回“假”，则表示中止此次枚举过程。"\
	"提示：在子程序名称前加“&”即可得到该子程序的“子程序指针”，如“&子程序1”就是“子程序1”的子程序指针。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_SUB_PTR,
/*default*/	0,
/*state*/	NULL,
	},
};

#define Global_EnumerateFiles_CmdInfo \
	{\
/*ccname*/	_WT("枚举文件"),\
/*egname*/	_WT("EnumerateFiles"),\
/*explain*/	_WT("在指定的目录内枚举所有文件，每枚举到一个文件，立刻调用用户提供的处理子程序（并传入文件名称）。"\
	"用户只需提供一个符合要求的处理子程序，就可以很方便地递归处理指定目录下的所有文件。"),\
/*category*/13,\
/*state*/	CMD_SUPPORT_OS,\
/*ret*/		_SDT_NULL,\
/*reserved*/0,\
/*level*/	LVL_SIMPLE,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_EnumerateFiles_ArgInfo) / sizeof(Global_EnumerateFiles_ArgInfo[0]),\
/*arg lp*/	Global_EnumerateFiles_ArgInfo,\
	}

#endif

typedef BOOL (WINAPI *EnumerateFunc) (LPCSTR str);

//本函数已被弃用，改用doFileEnumeration()
//枚举目录或文件，参数doFiles为TRUE表示枚举文件，为FALSE表示枚举目录
static BOOL s_bUserBreak = FALSE;
void doEnumeration(LPSTR lpPath, BOOL bRecursion, BOOL bDoFiles, EnumerateFunc pFunc)
{
	try{
	//-------------------------------------------------------------------------
	if(s_bUserBreak) return;

	int len = strlen(lpPath);
	if(lpPath==NULL || len<=0) return;

	NotifySys(NRS_DO_EVENTS, 0,0);
	
	char path[1024];
	strcpy(path, lpPath);
	if(lpPath[len-1] != '\\')
		strcat(path, "\\");
	strcat(path, "*.*");

	
	MDATA_INF RetData; MDATA_INF ArgInf[2];
	char buf[1024]; BOOL bUserRet=TRUE;
	
	//搜索当前目录下的文件
	if(bDoFiles)
	{
		ArgInf[0].m_dtDataType = SDT_TEXT; ArgInf[0].m_pText = path;
		//1、#只读文件； 2、#隐藏文件； 4、#系统文件； 16、#子目录； 32、#存档文件
		ArgInf[1].m_dtDataType = SDT_INT;  ArgInf[1].m_int = (bDoFiles ? 1+2+4+32 : 16);
		CallKrnlnFunc(fi_dir, &RetData, 2, ArgInf); 
		
		
		while(RetData.m_pText!=NULL && strlen(RetData.m_pText)>0)
		{
			strcpy(buf, path);
			strcat(buf, RetData.m_pText);
			if(pFunc)
			{
				bUserRet = pFunc(buf); 
				if(bUserRet==FALSE) { s_bUserBreak=TRUE; return; }
			}
			
			ArgInf[0].m_dtDataType = _SDT_NULL; ArgInf[0].m_pText = NULL; //不提供本参数
			//1、#只读文件； 2、#隐藏文件； 4、#系统文件； 16、#子目录； 32、#存档文件
			ArgInf[1].m_dtDataType = SDT_INT;  ArgInf[1].m_int = (bDoFiles ? 1+2+4+32 : 16);
			CallKrnlnFunc(fi_dir, &RetData, 2, ArgInf);
		}
	}

	
	if(bRecursion == FALSE) return;

	//-------------------------------------------------------------------------
	//下面递归搜索子目录 
	
	ArgInf[0].m_dtDataType = SDT_TEXT; ArgInf[0].m_pText = path;
	ArgInf[1].m_dtDataType = SDT_INT;  ArgInf[1].m_int = 16; //16、#子目录；
	CallKrnlnFunc(fi_dir, &RetData, 2, ArgInf); 

	while(RetData.m_pText!=NULL && strlen(RetData.m_pText)>0)
	{
		if(RetData.m_pText[strlen(RetData.m_pText)]!='.') 
		{
			strcpy(buf, path);
			strcat(buf, RetData.m_pText);
			if(pFunc && !bDoFiles)
			{
				bUserRet = pFunc(buf); 
				if(bUserRet==FALSE)
				{
					s_bUserBreak = TRUE; return;
				}
			}
			doEnumeration(buf, bRecursion, bDoFiles, pFunc); //递归调用本函数
		}

		ArgInf[0].m_dtDataType = _SDT_NULL; ArgInf[0].m_pText = NULL; //不提供本参数
		ArgInf[1].m_dtDataType = SDT_INT;  ArgInf[1].m_int = 16; //16、#子目录；
		CallKrnlnFunc(fi_dir, &RetData, 2, ArgInf);
	}

	
	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}


void doFileEnumeration(LPSTR lpPath, BOOL bRecursion, BOOL bEnumFiles, EnumerateFunc pFunc)
{
	try{
	//-------------------------------------------------------------------------
	if(s_bUserBreak) return;

	int len = strlen(lpPath);
	if(lpPath==NULL || len<=0) return;

	NotifySys(NRS_DO_EVENTS, 0,0);
	
	char path[MAX_PATH];
	strcpy(path, lpPath);
	if(lpPath[len-1] != '\\') strcat(path, "\\");
	strcat(path, "*");

	WIN32_FIND_DATA fd;
	HANDLE hFindFile = FindFirstFile(path, &fd);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile); return;
	}

	char tempPath[MAX_PATH]; BOOL bUserReture=TRUE; BOOL bIsDirectory;

	BOOL bFinish = FALSE;
	while(!bFinish)
	{
		strcpy(tempPath, lpPath);
		if(lpPath[len-1] != '\\') strcat(tempPath, "\\");
		strcat(tempPath, fd.cFileName);

		bIsDirectory = ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);

		//如果是.或..
		if( bIsDirectory
			&& (strcmp(fd.cFileName, ".")==0 || strcmp(fd.cFileName, "..")==0)) 
		{		
			bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
			continue;
		}

		if(pFunc && bEnumFiles!=bIsDirectory)
		{
			bUserReture = pFunc(tempPath);
			if(bUserReture==FALSE)
			{
				s_bUserBreak = TRUE; ::FindClose(hFindFile); return;
			}
		}

		NotifySys(NRS_DO_EVENTS, 0,0);
		
		if(bIsDirectory && bRecursion) //是子目录
		{
			doFileEnumeration(tempPath, bRecursion, bEnumFiles, pFunc);
		}

		bFinish = (FindNextFile(hFindFile, &fd) == FALSE);
	}

	::FindClose(hFindFile);

	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}

DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_EnumerateFiles)
{
	try{
	//-------------------------------------------------------------------------

		s_bUserBreak = FALSE;
		doFileEnumeration(pArgInf[0].m_pText, pArgInf[1].m_bool, TRUE, 
						  (EnumerateFunc)pArgInf[3].m_dwSubCodeAdr);

	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}

//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_EnumerateDirs_ArgInfo[] =
{
	{
/*name*/	_WT("要搜索的目录"),
/*explain*/	_WT("目录文本末尾可以有也可以没有“\\”字符。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_TEXT,
/*default*/	0,
/*state*/	NULL,
	},
	{
/*name*/	_WT("是否搜索子目录"),
/*explain*/	_WT("如果为“真”，则搜索范围为指定目录及其中的所有子目录（递归）；否则只在指定目录内搜索。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_BOOL,
/*default*/	1,
/*state*/	AS_HAS_DEFAULT_VALUE,
	},
	{
/*name*/	_WT("保留参数"),
/*explain*/	_WT(""),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_INT,
/*default*/	0,
/*state*/	AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
/*name*/	_WT("用户处理子程序"),
/*explain*/	_WT("用户提供的子程序必须符合以下要求：1、具有“逻辑型”返回值；2、有且只有一个“文本型”参数（用于接收子目录名称）。"\
	"在一次枚举过程中，该子程序会被多次调用（每枚举到一个子目录就调用一次）。"\
	"用户可根据参数传入的“全路径目录名称”（目录文本末尾总是不含“\\”字符）对该子目录进行所需操作（读写删除等不限）。"\
	"如果在此子程序中返回“假”，则表示中止此次枚举过程。"\
	"提示：在子程序名称前加“&”即可得到该子程序的“子程序指针”，如“&子程序1”就是“子程序1”的子程序指针。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_SUB_PTR,
/*default*/	0,
/*state*/	NULL,
	},
};

#define Global_EnumerateDirs_CmdInfo \
	{\
/*ccname*/	_WT("枚举子目录"),\
/*egname*/	_WT("EnumerateDirs"),\
/*explain*/	_WT("枚举指定目录内的所有子目录，每枚举到一个子目录，立刻调用用户提供的处理子程序（并传入子目录名称）。"\
	"用户只需提供一个符合要求的处理子程序，就可以很方便地递归处理指定目录下的所有子目录。"),\
/*category*/13,\
/*state*/	CMD_SUPPORT_OS,\
/*ret*/		_SDT_NULL,\
/*reserved*/0,\
/*level*/	LVL_SIMPLE,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_EnumerateDirs_ArgInfo) / sizeof(Global_EnumerateDirs_ArgInfo[0]),\
/*arg lp*/	Global_EnumerateDirs_ArgInfo,\
	}

#endif

DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_EnumerateDirs)
{
	try{
	//-------------------------------------------------------------------------

		s_bUserBreak = FALSE;
		doFileEnumeration(pArgInf[0].m_pText, pArgInf[1].m_bool, FALSE, 
						  (EnumerateFunc)pArgInf[3].m_dwSubCodeAdr);

	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}


//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_CallFunction_ArgInfo[] =
{
	{
/*name*/	_WT("推迟时间(单位: 毫秒)"),
/*explain*/	_WT("如果本参数小于或等于0，则子程序将被立刻调用。"
	"本参数最大可设置为 2147483647(0x7fffffff) 毫秒（约 596 小时）。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_INT,
/*default*/	0,
/*state*/	NULL,
	},
	{
/*name*/	_WT("子程序指针或子程序地址"),
/*explain*/	_WT("提示：在子程序名称前加“&”即可得到该子程序的“子程序指针”，如“&子程序1”就是“子程序1”的子程序指针；"
				"子程序地址可通过“特殊功能支持库”中的“取子程序地址()”命令获取，"
				"到整数(&子程序1)返回的也是子程序1的执行地址，"
				"也可通过API“GetProcAddress()”获取，或由外部程序传入。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	NULL,
	},
	{
/*name*/	_WT("保留参数"),
/*explain*/	_WT(""),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
/*name*/	_WT("返回值"),
/*explain*/	_WT("本参数用于接收子程序被调用后的返回值；如果该子程序没有返回值或不需要接收返回值，请省略本参数。"
	"提供本参数时，请务必提供准确的数据类型，否则所得结果可能不正确。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	AS_RECEIVE_VAR|AS_DEFAULT_VALUE_IS_EMPTY,
	},
	{
/*name*/	_WT("参数"),
/*explain*/	_WT("本参数值将作为参数传入被调用子程序。如果相应子程序没有参数，请省略本参数；如果相应子程序有多个参数，请重复提供本参数。"
	"请务必提供准确的参数类型和参数个数，否则后果不可预知。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	AS_DEFAULT_VALUE_IS_EMPTY,
	},
};

#define Global_CallFunction_CmdInfo \
	{\
/*ccname*/	_WT("调用子程序"),\
/*egname*/	_WT("CallFunction"),\
/*explain*/	_WT("以stdcall方式(即WINAPI或Pascal)调用指定的子程序（子程序指针，或子程序地址）并取回被调用子程序的返回值，"\
	"支持任意多个参数，参数支持除字节集之外的其它所有基本数据类型。"\
	"调用子程序之前，您需要明确地知道它的参数类型和参数个数，以及返回值类型，"\
	"必要时使用“到字节”“到短整数”“到长整数”“到小数”“到数值”等命令强制转换数据类型。"\
	"本命令返回“真”仅表示调用过程中没有出异常情况，并不表示被调用的子程序执行成功。"\
	"目前已知问题：参数同时含有“文本型”和“双精度小数型（日期时间型）”时，会导致参数传递错误。疑惑中……请汇编高手帮忙"),\
/*category*/30,\
/*state*/	CT_IS_HIDED|CMD_SUPPORT_OS|CT_ALLOW_APPEND_NEW_ARG,\
/*ret*/		SDT_BOOL,\
/*reserved*/0,\
/*level*/	LVL_HIGH,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_CallFunction_ArgInfo) / sizeof(Global_CallFunction_ArgInfo[0]) - 1,\
/*arg lp*/	&(Global_CallFunction_ArgInfo[1]),\
	}

#endif

//基本类型返回值,其它类型返回地址
//bytes中返回数据的字节数
#define		CF_VALUE_TYPE		INT
inline void CallFunction_GetValue(PMDATA_INF pMData, CF_VALUE_TYPE& HiValue, CF_VALUE_TYPE& LowValue, int& bytes)
{
	HiValue = LowValue = 0;
	if(pMData->m_dtDataType == _SDT_NULL) return;
	bytes = 4;
	
	switch(pMData->m_dtDataType) 
	{
	case SDT_INT: 
		LowValue = (CF_VALUE_TYPE)pMData->m_int; break;
	case SDT_SHORT: 
		LowValue = (CF_VALUE_TYPE)pMData->m_short; bytes = 2; break;
	case SDT_BYTE:
		LowValue = (CF_VALUE_TYPE)pMData->m_byte; bytes = 1; break;
	case SDT_INT64:
		{
			LowValue = (CF_VALUE_TYPE)_GetIntByIndex(&(pMData->m_int64), 0);
			HiValue  = (CF_VALUE_TYPE)_GetIntByIndex(&(pMData->m_int64), 1);
			bytes = 8;
			break;
		}
	case SDT_FLOAT: 
		//LowValue = (CF_VALUE_TYPE)pMData->m_float;  //这样肯定是不行的
		{
			//注意float转int的处理方式(通过ST0寄存器中转一下), 参考了VC编译器生成的汇编代码
			float f = pMData->m_float;
			int i = 0;
			__asm fld f;
			__asm fst i;
			LowValue = (CF_VALUE_TYPE)i;
			bytes = 4;
			break;
		}
	case SDT_DOUBLE:
	case SDT_DATE_TIME:
		{
			LowValue = (CF_VALUE_TYPE)_GetIntByIndex(&(pMData->m_double), 0);
			HiValue  = (CF_VALUE_TYPE)_GetIntByIndex(&(pMData->m_double), 1);

			//LowValue = *((CF_VALUE_TYPE*)(&(pMData->m_double)));
			//HiValue = *((CF_VALUE_TYPE*)((LPBYTE)(&(pMData->m_double)) + sizeof(CF_VALUE_TYPE)));

			//-- 前后三种方式经证实都是正确的

			/*
			int temp[2];
			double d = pMData->m_double;
			__asm fld qword ptr d;
			__asm fst qword ptr temp;
			LowValue = temp[0];
			HiValue  = temp[1];
			*/

			bytes = 8;
			break;
		}
	case SDT_BOOL:
		LowValue = (CF_VALUE_TYPE)(pMData->m_bool?1:0); bytes = 4; break; 
	case SDT_TEXT:
		LowValue = (CF_VALUE_TYPE)(DWORD)pMData->m_pText; break;
	case SDT_BIN:
	{
		if(pMData->m_pBin == NULL) //! 字节集为空的情况!
		{
			LowValue = (CF_VALUE_TYPE)0; break;
		}
		LowValue = (CF_VALUE_TYPE)(DWORD)(LPBYTE)_GetPointerByIndex(pMData->m_pBin, 2);
		break;
	}
	case SDT_SUB_PTR:
		LowValue = (CF_VALUE_TYPE)((DWORD)pMData->m_dwSubCodeAdr); break;
	case _SDT_NULL:
		LowValue = (CF_VALUE_TYPE)0; break;
	case SDT_STATMENT:
		//todo: 执行子语句并取其值
		LowValue = (CF_VALUE_TYPE)0; //!!!
		break;
	default: LowValue = (CF_VALUE_TYPE)pMData->m_int; break;
	}
}
//将寄存器EAX或ST0中的值写入pMData
void CallFunction_GetReturnedValue(PMDATA_INF pMData, int eaxValue,int edxValue,double stValue)
{
	if(pMData->m_dtDataType == _SDT_NULL) return;

	switch(pMData->m_dtDataType) 
	{
	case SDT_INT: case SDT_SHORT: case SDT_BYTE:
		*(pMData->m_pInt) = eaxValue;
		break;
	case SDT_INT64:
		{
			//EAX在低位, EDX在高位, 组成double
			int temp[2]; 
			temp[0]=eaxValue; temp[1]=edxValue;
			*(pMData->m_pInt64) = *(INT64*)temp;
			break;
		}
	case SDT_FLOAT: 
		//根据VC编译器生成的代码，返回的浮点数(float,double)应该在ST0中；看来易编译器与其不同。
		//*(pMData->m_pFloat) = (float)stValue; 
		{
			float f = 0;
			__asm fld eaxValue;
			__asm fst f;
			*(pMData->m_pFloat) = f;			
		}
		break;
	case SDT_DOUBLE:
	case SDT_DATE_TIME:
		//根据VC编译器生成的代码，返回的浮点数(float,double)应该在ST0中；看来易编译器与其不同。
		//*(pMData->m_pDouble) = stValue;
		{
			//EAX在低位, EDX在高位, 组成double
			int temp[2]; 
			temp[0]=eaxValue; temp[1]=edxValue;
			*(pMData->m_pDouble) = *(double*)temp;
		}
		break;
	case SDT_BOOL:
		*(pMData->m_pBool) = (eaxValue!=0);
		break;
	case SDT_TEXT:
		*(pMData->m_ppText) = CloneTextData((char*)eaxValue);
		break;
	case SDT_BIN:
		//pMData->m_pBin = CloneBinData((LPBYTE)intValue, n); //? 无法知道字节集长度!
		break;
	case SDT_SUB_PTR:
		break;
	case _SDT_NULL:
		break;
	case SDT_STATMENT:
		break;

	default: 
		break;
	}
}

//调用子程序, liigo原创
//pMData: "子程序指针或地址"参数的内存地址，nArgCount: 被调用子程序的参数个数
BOOL Global_CallFunction(PMDATA_INF pFuncMData, INT nArgCount)
{
	try{
	//-------------------------------------------------------------------------

	//取被调用子程序的执行地址(pFunc)
	CF_VALUE_TYPE pFunc = NULL; //pArgInf[0].m_dwSubCodeAdr;
	CF_VALUE_TYPE temp  = 0;
	CallFunction_GetValue(pFuncMData, temp, pFunc, temp);
	if(pFunc == 0) return FALSE;

	//返回值
	PMDATA_INF pReturnMData = &pFuncMData[2];
	//参数
	PMDATA_INF pParamsMData = &pFuncMData[3];

	//参数依次反序压栈
	CF_VALUE_TYPE  HiValue=0, LowValue=0;
	int bytes = 0;
	int byteValue  = 0;
	int shortValue = 0;
	if(!(pParamsMData[0].m_dtDataType==_SDT_NULL && nArgCount<=1)) //第一个参数被省略表示没有参数
	{
		for(int i=nArgCount-1; i>=0; i--) //反序压栈
		{
			CallFunction_GetValue(&pParamsMData[i], HiValue,LowValue, bytes); //函数调用会影响参数栈吗?
			
			//经查看VC编译器生成的汇编代码，参数压栈总是以4字节为单位
			byteValue = (int)(byte)LowValue; shortValue = (int)(short)LowValue;

			//如果是8字节数值的话,先高位入栈,后低位入栈
			if(bytes == 8)
			{
				__asm PUSH HiValue;
				__asm PUSH LowValue;
			}
			else if(bytes == 4)
				__asm PUSH LowValue;
			else if(bytes == 2)
				__asm PUSH shortValue;
			else if(bytes == 1)
				__asm PUSH byteValue; 
			else
				ASSERT(FALSE);
		}
	}

	//调用子程序
	__asm CALL pFunc; 

	//取被调用子程序的返回值 (须在CALL之后立刻读取)
	int eaxValue = 0, edxValue = 0;
	double stValue = 0.0;
	__asm mov eaxValue, eax;
	__asm mov edxValue, edx;
	__asm fst stValue; //! 取ST0寄存器中存储的浮点数值(float,double)

	if(pReturnMData->m_dtDataType != _SDT_NULL)
		CallFunction_GetReturnedValue(pReturnMData, eaxValue,edxValue,stValue);

	return TRUE;

	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return FALSE; }

}

DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_CallFunction)
{
	pRetData->m_bool = FALSE;
	try{
	//-------------------------------------------------------------------------

	BOOL bOK = Global_CallFunction(pArgInf, nArgCount-3);

	pRetData->m_bool = bOK;
		
	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}

//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

#define Global_DelayCallFunction_CmdInfo \
	{\
/*ccname*/	_WT("推迟调用子程序"),\
/*egname*/	_WT("DelayCallFunction"),\
/*explain*/	_WT("本命令被执行时，在内部启动一个计时器，然后立刻返回（以便易语言继续执行后面的程序代码），"\
	"当计时时间到时才调用参数所指定的子程序。如上所述，本命令不会阻塞易语言程序的运行。"\
	"请注意不要在本命令已调用但相应子程序尚未执行这一段时间内再次调用本命令（将直接返回“假”）。"\
	"本命令的第四个参数“返回值”目前不予使用。"),\
/*category*/30,\
/*state*/	CT_IS_HIDED|CMD_SUPPORT_OS|CT_ALLOW_APPEND_NEW_ARG,\
/*ret*/		SDT_BOOL,\
/*reserved*/0,\
/*level*/	LVL_HIGH,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_CallFunction_ArgInfo) / sizeof(Global_CallFunction_ArgInfo[0]),\
/*arg lp*/	Global_CallFunction_ArgInfo,\
	}

#endif

void* g_FuncDatas = NULL;
INT   g_ArgCount   = 0;
bool g_bIsTiming = false;
UINT g_TimerID = 0; 

VOID CALLBACK 
DelayCallFunction_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	try
	{
		::KillTimer(NULL, g_TimerID);
		g_bIsTiming = false;
		if(g_FuncDatas)
			Global_CallFunction(PMDATA_INF(g_FuncDatas), g_ArgCount);
	}
	catch(...)
	{
	}
}

DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_DelayCallFunction)
{
	try{
	//-------------------------------------------------------------------------
	pRetData->m_bool = FALSE;

	if(g_bIsTiming) return;
	
	INT iDelay = pArgInf[0].m_int;
	if(iDelay > 0)
	{
		g_TimerID = ::SetTimer(NULL, 0, iDelay, (TIMERPROC)DelayCallFunction_TimerProc);
		g_bIsTiming = true;
		
		//把相关的数据保存起来，留待后用。Todo: 要考虑引用的问题。
		if(g_FuncDatas) free(g_FuncDatas);
		int nBytes = sizeof(MDATA_INF) * (nArgCount-1);
		g_FuncDatas = malloc(nBytes);
		memcpy(g_FuncDatas, &(pArgInf[1]), nBytes); //这样恐怕有问题;)
		g_ArgCount = nArgCount - 4;

		pRetData->m_bool = TRUE;
	}
	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}

//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_Wait_ArgInfo[] =
{
	{
/*name*/	_WT("要等待的时间(毫秒)"),
/*explain*/	_WT("提示：1000毫秒等于1秒。"),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	SDT_INT,
/*default*/	0,
/*state*/	NULL,
	},
};

#define Global_Wait_CmdInfo \
	{\
/*ccname*/	_WT("等待"),\
/*egname*/	_WT("Wait"),\
/*explain*/	_WT("等待一段时间，然后继续执行后面的程序代码。"\
	"与易语言核心支持库中的“延时()”命令不同的是，本命令在等待期间允许用户进行其它操作（如点击按钮等）。感谢易友王军提议增加此命令。"),\
/*category*/15,\
/*state*/	CT_IS_HIDED|CMD_SUPPORT_OS,\
/*ret*/		_SDT_NULL,\
/*reserved*/0,\
/*level*/	LVL_SIMPLE,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_Wait_ArgInfo) / sizeof(Global_Wait_ArgInfo[0]),\
/*arg lp*/	Global_Wait_ArgInfo,\
	}

#endif

DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_Wait)
{
	try{
	//-------------------------------------------------------------------------

	if(pArgInf[0].m_int <= 0) return;

	DWORD iEnd = ::GetTickCount() + pArgInf[0].m_int;

	while(1)
	{
		NotifySys(NRS_DO_EVENTS, 0,0);
		NotifySys(NRS_DO_EVENTS, 0,0);

		if(::GetTickCount() > iEnd) break;

		NotifySys(NRS_DO_EVENTS, 0,0);
		NotifySys(NRS_DO_EVENTS, 0,0);
	}

	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}

//-----------------------------------------------------------------------------
// 方法参数、信息及实现代码

#ifndef __E_STATIC_LIB

ARG_INFO Global_GetVarAddr_ArgInfo[] =
{
	{
/*name*/	_WT("变量"),
/*explain*/	_WT(""),
/*bmp inx*/	0,
/*bmp num*/	0,
/*type*/	_SDT_ALL,
/*default*/	0,
/*state*/	AS_RECEIVE_VAR_OR_ARRAY,
	},
};

#define Global_GetVarAddr_CmdInfo \
	{\
/*ccname*/	_WT("取变量数据地址"),\
/*egname*/	_WT("GetVarAddr"),\
/*explain*/	_WT("取指定变量中数据的内存地址。返回值为0表示没有取到数据地址。"\
	"对文本型或字节集型变量而言，如果其长度为0，将返回0；"\
	"对数组变量而言，如果其成员数为0，也将返回0。"\
	"对于文本型或字节集型数组变量，返回值为“数组各成员数据的地址集合”的首地址，而非实际数据地址"\
	"（因为文本型或字节集型数组并未将所有成员中的文本或字节集存储于同一块内存区域）；"\
	"但仍可通过“以数组成员作为参数调用本命令”来获取各数组成员中的文本或字节集内存地址。"\
	"注意：一旦变量超出其作用域范围，或文本型/字节集型变量被重新赋值，或数组变量被重新赋值，"\
	"先前取到的变量地址将变成无效地址，再对该地址进行读写操作很有可能导致程序崩溃。"),\
/*category*/30,\
/*state*/	CT_IS_HIDED|CMD_SUPPORT_OS,\
/*ret*/		SDT_INT,\
/*reserved*/0,\
/*level*/	LVL_HIGH,\
/*bmp inx*/	0,\
/*bmp num*/	0,\
/*ArgCount*/sizeof(Global_GetVarAddr_ArgInfo) / sizeof(Global_GetVarAddr_ArgInfo[0]),\
/*arg lp*/	Global_GetVarAddr_ArgInfo,\
	}

#endif

DEFINE_KRNLNEX_EXECUTE_CMD (fn_Global_GetVarAddr)
{
	pRetData->m_int = 0;
	try{
	//-------------------------------------------------------------------------

	PMDATA_INF pParam = &(pArgInf[0]);
	BOOL bIsArray = ((pParam->m_dtDataType & DT_IS_ARY) !=0 );
	if(bIsArray) pParam->m_dtDataType &= (~DT_IS_ARY);

	int nAddr = 0;

	if(bIsArray)
	{
		int nLen = 0;
		nAddr = (int)GetAryElementInf(*(pParam->m_ppAryData), &nLen); //取数组数据地址
		if(nLen == 0) nAddr = 0;
	}
	else
	{
		switch(pParam->m_dtDataType)
		{
		case SDT_BYTE:	
		case SDT_SHORT:
		case SDT_INT:
		case SDT_BOOL:
		case SDT_INT64:
		case SDT_FLOAT:
		case SDT_DOUBLE:
		case SDT_DATE_TIME:
		case SDT_SUB_PTR:
		case SDT_STATMENT:
			nAddr =  (int)pParam->m_pByte; //union, 地址应该是相同的吧
			break;
		
		/*
		//没什么价值
		case SDT_STATMENT:
			nAddr = (int)pParam->m_pStatment->m_dwStatmentSubCodeAdr; //子语句返回执行地址
			break;
		*/
		
		case SDT_TEXT:
			{
				char* pText = *(pParam->m_ppText);
				nAddr = ((pText==NULL || strlen(pText)<=0) ? 0 : (int)pText); //如果文本不为空, 取其地址
				break;
			}
		case SDT_BIN:
			{
				LPBYTE pBin = *(pParam->m_ppBin);
				
				if(pBin == NULL)
				{
					nAddr = 0;
				}
				else
				{
					nAddr = ((_GetIntByIndex(pBin, 1)==0) ? 0 : (int)pBin + 8); //如果字节集不为空, 取其地址
				}
				break;
			}
			
		default: nAddr = 0;
		}
	}

	pRetData->m_int = nAddr;

	//-------------------------------------------------------------------------
	}catch(...){ ASSERT(FALSE); return; }
}



#endif //#ifndef GLOBAL_FUNCTION_HPP
