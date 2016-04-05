// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"


#include <afx.h>
#include <afxdisp.h>
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>
#include<Pdh.h>
#include<iostream>
#include<PDHMsg.h>
#pragma comment(lib, "Pdh.lib")
#include <iphlpapi.h> 
#pragma comment ( lib, "iphlpapi.lib")
#include <vector>
#include <time.h>
#include <map>
using namespace std;
// TODO:  在此处引用程序需要的其他头文件



