// NewLib.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "share.hpp"


#ifdef __cplusplus
	#define EXTERN_C extern "C" 
#else
	#define EXTERN_C 
#endif

#define DEFINE_KRNLNEX_EXECUTE_CMD(fnName) \
	EXTERN_C void krnlnex##_##fnName (PMDATA_INF pRetData, INT nArgCount, PMDATA_INF pArgInf)


#if defined(__E_STATIC_LIB) || defined(__COMPILE_FNR)

	#define KRNLNEX_CMD(name)  krnlnex_##name  // 对于不同的库, 请做相应的名称修改
	#define KRNLNEX_NULL_CMD(name)  NULL

#else if defined(__cplusplus)

    static CFreqMem _g_CmdNames;

    static PFN_EXECUTE_CMD _RecordCmdName (PFN_EXECUTE_CMD func, const TCHAR* szFuncName)
    {
        _g_CmdNames.AddDWord ((DWORD)szFuncName);
        return func;
    }

    static const TCHAR** _GetCmdNames ()
    {
        return (const TCHAR**)_g_CmdNames.GetPtr ();
    }

    #define KRNLNEX_CMD(name)  _RecordCmdName (krnlnex_##name, "krnlnex_" #name)  // 对于不同的库, 请做相应的名称修改
    #define KRNLNEX_NULL_CMD(name)  _RecordCmdName(NULL, NULL)

#endif

#define KRNLNEX_NULL_CMD_1  KRNLNEX_NULL_CMD(NULL)
#define KRNLNEX_NULL_CMD_2  KRNLNEX_NULL_CMD_1,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_3  KRNLNEX_NULL_CMD_2,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_4  KRNLNEX_NULL_CMD_3,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_5  KRNLNEX_NULL_CMD_4,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_6  KRNLNEX_NULL_CMD_5,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_7  KRNLNEX_NULL_CMD_6,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_8  KRNLNEX_NULL_CMD_7,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_9  KRNLNEX_NULL_CMD_8,KRNLNEX_NULL_CMD_1
#define KRNLNEX_NULL_CMD_10 KRNLNEX_NULL_CMD_9,KRNLNEX_NULL_CMD_1

#include "Global_functions.hpp"


#ifndef __E_STATIC_LIB

//=============================================================================
// 支持库定义开始

INT WINAPI RunAddIn(INT nIndex)
{
    switch(nIndex) 
    {
    case 0: 
        break;
    default:
        break;
    }
    return 0;
}


//数据类型列表
LIB_DATA_TYPE_INFO s_DataType[]=
{
	MASK_DATATYPE_INFO_1,

};



///////////////////////////////////////////////////////////////////////////////
//*** 常量定义信息:
// !!! 注意常量值由于已经运用到程序中，所以顺序及值绝对不能改动。

LIB_CONST_INFO s_ConstInfo [] =
{
	{_WT("支持库代号"),_WT("LibAlias"),NULL,LVL_SIMPLE,CT_TEXT,_T("DebugHelper"),0,},

};


///////////////////////////////////////////////////////////////////////////////
// 所有的命令及方法列表

static CMD_INFO s_CmdInfo[] =
{
	Global_All_CmdInfo

};


///////////////////////////////////////////////////////////////////////////////
// 所有的命令及方法执行函数表

PFN_EXECUTE_CMD s_RunFunc [] =	// 索引应与s_CmdInfo中的命令定义顺序对应
{
	Global_All_functions

};

#endif


EXTERN_C INT WINAPI krnlnex_ProcessNotifyLib (INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
#ifndef __E_STATIC_LIB
	if(nMsg == NL_GET_CMD_FUNC_NAMES)
		return (INT) _GetCmdNames();
	else if(nMsg == NL_GET_NOTIFY_LIB_FUNC_NAME)
		return (INT) "krnlnex_ProcessNotifyLib";
	else if(nMsg == NL_GET_DEPENDENT_LIBS)
		return (INT) NULL;
#endif
	return ProcessNotifyLib(nMsg, dwParam1, dwParam2);
}

#ifndef __E_STATIC_LIB

///////////////////////////////////////////////////////////////////////////////
// 库定义开始
static char* s_szInfo = "This is a runtime library file for EPL applications. The EPL is a software development environment. For details please visit www.dywt.com.cn/info";

LIB_INFO s_LibInfo =
{
/*Lib Format Ver*/		LIB_FORMAT_VER,		// 保留未用。

// 本支持库的GUID串
#define		LI_LIB_GUID_STR	"{A00B8D30-A5AB-4dfe-9546-9E636E711B75}"
/*guid str*/			_T (LI_LIB_GUID_STR),

/*m_nMajorVersion*/		2,
/*m_nMinorVersion*/		0,
/*m_nBuildNumber*/		1,
/*m_nRqSysMajorVer*/		3,
/*m_nRqSysMinorVer*/		7,
/*m_nRqSysKrnlLibMajorVer*/	3,
/*m_nRqSysKrnlLibMinorVer*/	7,

/*name*/				_T ("枚举文件子目录支持库"), //拟改为: 系统核心增强支持库(辅助调试支持库)
/*lang*/				__COMPILE_LANG_VER,
/*explain*/				_WT("提供“枚举文件()”和“枚举子目录()”命令，可以很方便地递归处理指定目录下的所有文件或子目录。"
	"\r\n    1.0#0版提供“调试输出()”“验证()”等命令，为调试易语言程序提供便利，增强了易语言核心库中的相关调试命令（“输出调试文本()”“检查()”）。"
	"\r\n    1.1#0版增加了“枚举文件()”和“枚举子目录()”命令（对易语言核心库中“寻找文件()”命令的增强），可以很方便地递归处理指定目录下的所有文件或子目录。"
	"\r\n    1.2#0版增加了“调用子程序()”命令，支持任意多个参数，参数支持除字节集之外的其它所有基本数据类型。"
	"\r\n    1.3#0版增加了“等待()”命令（对易语言核心库中“延时()”命令的增强），在等待期间程序可以响应用户操作。感谢易友王军提议。"
	"\r\n    1.5#0版完善了“调用子程序()”，支持调用函数地址，可以取到调用后的返回值，并修正了部分参数传递错误；"
	"此外增加了命令“推迟调用子程序()”与“取变量数据地址()”。如果不出意外，这将是“辅助调试支持库”的最后一个版本。"
	"\r\n    2.0#0版，支持静态编译，并隐藏多个与易语言官方特殊功能支持库重复的命令（如“调试输出”“调用子程序”“取变量数据地址”等），只保留“枚举文件()”“枚举子目录()”两条命令，支持库也重命名为“枚举文件子目录支持库”，但仍保持向后兼容。"
	"\r\n    易语言核心支持库与本支持库所处情形不同，核心库需考虑命令的通用性、扩展性及向后兼容性，而本库只需考虑怎样让用户最方便的使用。"
	"\r\n    本支持库后续版本还打算继续增强易语言系统核心支持库中的其它命令，您对哪个命令不满意，欢迎提议。"),
/*dwState*/				LIB_SUPPORT_OS,

/*szAuthor*/	_WT("liigo"),
/*szZipCode*/	_WT(""),
/*szAddress*/	_WT(""),
/*szPhoto*/		_WT(""),
/*szFax*/		_WT(""),
/*szEmail*/		_WT("com.liigo@gmail.com"),
/*szHomePage*/	_WT("http://blog.csdn.net/liigo"),
/*szOther*/		_WT("祝您一帆风顺，心想事成！"),

/*type count*/			0, //sizeof(s_DataType) / sizeof(s_DataType[0]),
/*PLIB_DATA_TYPE_INFO*/	NULL, //s_DataType,

/*CategoryCount*/ 30,	// 加了类别需加此值。
/*category*/// 类别说明表每项为一字符串,前四位数字表示图象索引号(从1开始,0无).
			_T("0001流程控制\0"
            "0014算术运算\0"
            "0005逻辑比较\0"
            "0015位运算\0"
            "0002变量操作\0"
            "0000数组操作\0"
            "0000环境存取\0"
            "0000拼音处理\0"
            "0000数值转换\0"
            "0000文本操作\0"
            "0000时间操作\0"
            "0000字节集操作\0"
            "0000磁盘操作\0"
            "0000文件读写\0"
            "0000系统处理\0"
            "0000媒体播放\0"
            "0000程序调试\0"
            "0000其他\0"
            "0021数据库\0"
            "0000网络通信\0"
            "0000控制台操作\0"
			"0000???\0"
			"0000???\0"
			"0000???\0"
			"0000???\0"
			"0000???\0"
			"0000???\0"
			"0000???\0"
			"0000???\0"
			"0000附加\0"
            "\0"),
/*CmdCount*/				sizeof (s_CmdInfo) / sizeof (s_CmdInfo [0]),
/*BeginCmd*/				s_CmdInfo,
/*m_pCmdsFunc*/             s_RunFunc,
/*pfnRunAddInFn*/			RunAddIn,
/*szzAddInFnInfo*/			"",

/*pfnNotify*/				krnlnex_ProcessNotifyLib,

/*pfnRunSuperTemplate*/		NULL,
/*szzSuperTemplateInfo*/	NULL,

/*nLibConstCount*/			0, //sizeof (s_ConstInfo) / sizeof (s_ConstInfo [0]),
/*pLibConst*/				NULL, //s_ConstInfo,

/*szzDependFiles*/			NULL,
};


// 支持库定义结束
//=============================================================================

/////////////////////////////////////////////////////////////////////////////
//导出函数

PLIB_INFO WINAPI GetNewInf ()
{
	ClearGlobalDatas();
	Get_Krnln_Functions_Address();

	return &s_LibInfo;
}

#endif
