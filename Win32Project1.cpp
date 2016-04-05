// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#ifndef   WS_EX_LAYERED 
#define   WS_EX_LAYERED                       0x00080000
#define   LWA_COLORKEY                         0x00000001 
#define   LWA_ALPHA                               0x00000002 
#endif   //   ndef   WS_EX_LAYERED 

bool TimerChanged = false;



struct Time
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
};

int monthday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

 vector<BYTE*> counts;
map<HWND, CString> combos;
map<HWND, HWND> combohwnd;
map<HWND, UINT_PTR> combotimer;
UINT_PTR Timercount = 10;
map<HWND, Time*>combotime;
map<HWND, int> warn;
map<HWND, CTimeSpan> combospan;
map<HWND, HWND>combohwnd2;



#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)
#define MAX_MSG_LENGTH 150

#define WIDTH 170
#define HEIGHT 220

#define WIDTH2 90
#define HEIGHT2 40

#define CHILDWIDTH 300
#define CHILDHEIGHT 30


#define CHILDWIDTH2 100
#define CHILDHEIGHT2 20

int Mode = 2;
double mb1=0, mb2=0;
HINSTANCE  g_hInstance = NULL;
HWND focuson = NULL;

HBRUSH colors[] = { 
	               CreateSolidBrush(RGB(254,67,101)),
				   CreateSolidBrush(RGB(131,175,155)),
		    	   CreateSolidBrush(RGB(248,147,29)),
				   CreateSolidBrush(RGB(56,13,49)),
				   CreateSolidBrush(RGB(114,111,128)),
				   CreateSolidBrush(RGB(217,104,49)),
				   CreateSolidBrush(RGB(6,128,67)),
				   CreateSolidBrush(RGB(38,157,128)),
				   CreateSolidBrush(RGB(69,137,148)),
				   CreateSolidBrush(RGB(137,157,192)),
				   CreateSolidBrush(RGB(98,65,24))
};

PMIB_IFTABLE	m_pTable = NULL;
DWORD	 m_dwAdapters = 0;
ULONG	 uRetCode = GetIfTable(m_pTable, &m_dwAdapters, TRUE);

DWORD	dwLastIn = 0;																					//上一秒钟的接收字节数
DWORD	dwLastOut = 0;																					//上一秒钟的发送字节数
DWORD	dwBandIn = 0;																					//下载速度
DWORD	dwBandOut = 0;

long long m_lSysTotalMem;
long m_nSysHandleCount;
long m_nSysThreadCount;
long long m_nSysProcessCount;
long long m_nSysCpu;
long long m_lSysAvailableMem;
long long m_lCoreMem1;
long long m_lCoreMem2;
long long m_lCommitMem1;
long long m_lCommitMem2;


LONGLONG GetTimerSecond(HWND hwnd)
{
	Time *t = combotime.find(hwnd)->second;
	CTime timenow = CTime::GetCurrentTime();
	CTime ct(t->year, t->month, t->day, t->hour, t->minute, 0);
	if (ct > timenow)
	{
		CTimeSpan tmSpan = ct - timenow;
		return tmSpan.GetTotalSeconds() * 1000;
	}
	else
		return 0;
}

int countstr(char* str, char* s)
{
	char* s1;
	char* s2;
	int count = 0;
	while (*str != '\0')
	{
		s1 = str;
		s2 = s;
		while (*s2 == *s1 && (*s2 != '\0') && (*s1!= '0'))
		{
			s2++;
			s1++;
		}
		if (*s2 == '\0')
			count++;
		str++;
	}
	return count;
}


void StrReplaceB(char* strSrc, char* strFind, char* strReplace)
{
	while (*strSrc != '\0')
	{
		if (*strSrc == *strFind)
		{
			if (strncmp(strSrc, strFind, strlen(strFind)) == 0)
			{
				int i = strlen(strFind);
				int j = strlen(strReplace);
				char* q = strSrc + i;
				char* p = q;//p、q均指向剩余字符串的首地址
				char* repl = strReplace;
				int lastLen = 0;
				while (*q++ != '\0')
					lastLen++;
				char* temp = new char[lastLen + 1];//临时开辟一段内存保存剩下的字符串,防止内存覆盖
				for (int k = 0; k < lastLen; k++)
				{
					*(temp + k) = *(p + k);
				}
				*(temp + lastLen) = '\0';
				while (*repl != '\0')
				{
					*strSrc++ = *repl++;
				}
				p = strSrc;
				char* pTemp = temp;//回收动态开辟内存
				while (*pTemp != '\0')
				{
					*p++ = *pTemp++;
				}
				delete temp;
				*p = '\0';
			}
			else
				strSrc++;
		}
		else
			strSrc++;
	}
}

VOID    CALLBACK TimerProc5(HWND hwnd, UINT, UINT iTimerID, DWORD)
{
	warn.find(hwnd)->second+=5;
	if (warn.find(hwnd)->second > 200)
		warn.find(hwnd)->second = 0;
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}

VOID    CALLBACK TimerProc6(HWND hwnd, UINT, UINT iTimerID, DWORD)
{
	
	if (combospan.find(GetParent(hwnd)) != combospan.end())
	{
		combospan.find(GetParent(hwnd))->second -= CTimeSpan(0, 0, 0, 1);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		if (combospan.find(GetParent(hwnd))->second.GetTotalSeconds() < 1)
			DestroyWindow(hwnd);
	}
}



VOID    CALLBACK TimerProc4(HWND hwnd, UINT, UINT iTimerID, DWORD)
{
	//重新测试时间:看是否到了计时的时间
	LONGLONG sec = GetTimerSecond(hwnd);
	//如果否 
	//按照原来的ID重建一个Timer（最大USER_TIMER_MAXIMUM）
	//如果是 
	if (sec < 1000)
	{
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		pair<HWND, int> p(hwnd, 0);
		warn.insert(p);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		KillTimer(hwnd, iTimerID);
		SetTimer(hwnd, iTimerID, 50, TimerProc5);
	}
	else
	{
		KillTimer(hwnd, iTimerID);
		if (sec >= USER_TIMER_MAXIMUM)
			SetTimer(hwnd, iTimerID, USER_TIMER_MAXIMUM, TimerProc4);
		else
			SetTimer(hwnd, iTimerID, sec, TimerProc4);
	}
	//建立一个新timer（逐渐变红） TimerProc5
	//把旧timer从map中去掉
	//将新timer加入map
	//自杀
}




VOID    CALLBACK TimerProc2(HWND hwnd, UINT, UINT, DWORD)
{
	RECT rect;
	GetWindowRect(hwnd,&rect);
	if (rect.bottom - rect.top > 40)
	{
		MoveWindow(hwnd, rect.left, rect.top, rect.right-rect.left-8, rect.bottom - rect.top - 18,true);
	}
	else
	{
		MoveWindow(hwnd, rect.left, rect.top, WIDTH2, HEIGHT2,true);
		KillTimer(hwnd, 2);
		Mode++;
	}
}

VOID    CALLBACK TimerProc3(HWND hwnd, UINT, UINT, DWORD)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	if (rect.right > SCREEN_WIDTH || rect.bottom > SCREEN_HEIGHT)
	{
		if (rect.right > SCREEN_WIDTH)
			rect.left = SCREEN_WIDTH - width;
		else
			rect.top = SCREEN_HEIGHT - height;
		MoveWindow(hwnd, rect.left, rect.top, width, height, false);
	}
	if (rect.bottom - rect.top < HEIGHT)
	{
		MoveWindow(hwnd, rect.left, rect.top, width + 8, height + 18, true);
	}
	else
	{
		MoveWindow(hwnd, rect.left, rect.top, WIDTH, HEIGHT, true);
		KillTimer(hwnd, 3);
		Mode--;
	}
}




VOID    CALLBACK TimerProc(HWND hwnd, UINT, UINT, DWORD)
{
	if (uRetCode != ERROR_NOT_SUPPORTED)
	{
		GetIfTable(m_pTable, &m_dwAdapters, TRUE);
		DWORD	dwInOctets = 0;
		DWORD	dwOutOctets = 0;
		//将所有端口的流量进行统计
		for (UINT i = 0; i < m_pTable->dwNumEntries; i++)
		{
			MIB_IFROW	Row = m_pTable->table[i];
			if (Row.dwPhysAddrLen)
			{
				IFTYPE type = Row.dwType;
				if (type == IF_TYPE_ETHERNET_CSMACD || type == IF_TYPE_SOFTWARE_LOOPBACK || type == IF_TYPE_IEEE80211)
				{
					for (int j = 0;j < counts.size();j++)
					{
						if (!memcmp(Row.bPhysAddr, counts[j], Row.dwPhysAddrLen))
							goto end;
					}
					dwInOctets += Row.dwInOctets;
					dwOutOctets += Row.dwOutOctets;
					BYTE *b = new BYTE[Row.dwPhysAddrLen];
					memcpy_s(b, sizeof(BYTE) * Row.dwPhysAddrLen, Row.bPhysAddr, sizeof(BYTE) * Row.dwPhysAddrLen);
					counts.push_back(b);
				end:;
				}
			}
		}
		if (dwInOctets == 0)
		{
			for (UINT i = 0; i < m_pTable->dwNumEntries; i++)
			{
				MIB_IFROW	Row = m_pTable->table[i];
			}
			for (int i = 0;i < counts.size();i++)
			{
				CString a;
				a = counts[i];
			}



		}
			dwBandIn = dwInOctets - dwLastIn;
		    dwBandOut = dwOutOctets - dwLastOut;
		for (int i = 0;i < counts.size();i++)
		{
			delete counts[i];
			counts[i] = NULL;
		}
		 vector<BYTE*>().swap(counts);

		if (dwLastIn <= 0)
		{
			dwBandIn = 0;
		}
		else
		{
			dwBandIn = dwBandIn / 1024;	
			if (dwBandIn > 1024)
			{
				mb1 = (double)dwBandIn / 1024;
			}
			else
				mb1 = 0;
		}

		if (dwLastOut <= 0)
		{
			dwBandOut = 0;
		}
		else
		{
			dwBandOut = dwBandOut / 1024;	
			if (dwBandOut > 1024)
				mb2 = (double)dwBandOut / 1024;
			else
				mb2 = 0;
		}
		    dwLastIn = dwInOctets;
		    dwLastOut = dwOutOctets;
	}
	if (Mode == 0)
	{
		HQUERY hQuery;

		HCOUNTER hcSysAvailableMem, hcSysCpu;
		HCOUNTER hcCommitTotal, hcCommitLimit;
		HCOUNTER hcKernelPaged, hcKernelNonpaged;
		HCOUNTER hcSysHandleCount, hcSysProcesses, hcSysThreads;

		PDH_STATUS lStatus = PdhOpenQuery(NULL, NULL, &hQuery);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}

		PdhAddCounter(hQuery, _T("\\Memory\\Available Bytes"), NULL, &hcSysAvailableMem);//可用内存
		PdhAddCounter(hQuery, _T("\\Processor(_Total)\\% Processor Time"), NULL, &hcSysCpu);
		PdhAddCounter(hQuery, _T("\\Process(_Total)\\Handle Count"), NULL, &hcSysHandleCount);
		PdhAddCounter(hQuery, _T("\\System\\Processes"), NULL, &hcSysProcesses);
		PdhAddCounter(hQuery, _T("\\System\\Threads"), NULL, &hcSysThreads);
		PdhAddCounter(hQuery, _T("\\Memory\\Committed Bytes"), NULL, &hcCommitTotal);
		PdhAddCounter(hQuery, _T("\\Memory\\Commit Limit"), NULL, &hcCommitLimit);
		PdhAddCounter(hQuery, _T("\\Memory\\Pool Paged Bytes"), NULL, &hcKernelPaged);
		PdhAddCounter(hQuery, _T("\\Memory\\Pool Nonpaged Bytes"), NULL, &hcKernelNonpaged);



		HCOUNTER hcCPU = NULL;

		HCOUNTER hcThreadCount = NULL;

		HCOUNTER hcHandleCount = NULL;

		HCOUNTER hcWorkingSet = NULL;

		HCOUNTER hcWorkingSetPeak = NULL;

		PDH_COUNTER_PATH_ELEMENTS elements;

		char szBuf[1024] = "";

		DWORD dwBufSize = 0;

		elements.szMachineName = NULL;

		elements.szObjectName = "Process";

		elements.szInstanceName = "0";

		elements.szParentInstance = NULL;

		elements.dwInstanceIndex = -1;

		elements.szCounterName = const_cast<char *>("% Processor Time");

		dwBufSize = sizeof(szBuf);

		PdhMakeCounterPath(&elements, szBuf, &dwBufSize, 0);

		lStatus = PdhAddCounter(hQuery, szBuf, NULL, &hcCPU);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}

		elements.szCounterName = const_cast<char *>("Thread Count");

		dwBufSize = sizeof(szBuf);

		PdhMakeCounterPath(&elements, szBuf, &dwBufSize, 0);

		lStatus = PdhAddCounter(hQuery, szBuf, NULL, &hcThreadCount);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}
		elements.szCounterName = const_cast<char *>("Handle Count");

		dwBufSize = sizeof(szBuf);

		PdhMakeCounterPath(&elements, szBuf, &dwBufSize, 0);

		lStatus = PdhAddCounter(hQuery, szBuf, NULL, &hcHandleCount);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}
		elements.szCounterName = const_cast<char *>("Working set");

		dwBufSize = sizeof(szBuf);

		PdhMakeCounterPath(&elements, szBuf, &dwBufSize, 0);

		lStatus = PdhAddCounter(hQuery, szBuf, NULL, &hcWorkingSet);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}
		elements.szCounterName = const_cast<char *>("Working set Peak");

		dwBufSize = sizeof(szBuf);

		PdhMakeCounterPath(&elements, szBuf, &dwBufSize, 0);

		lStatus = PdhAddCounter(hQuery, szBuf, NULL, &hcWorkingSetPeak);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}
		PDH_FMT_COUNTERVALUE cv;

		memset(&cv, 0, sizeof(PDH_FMT_COUNTERVALUE));

		lStatus = PdhCollectQueryData(hQuery);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}
		// CPU时间，必须等待一下
		Sleep(50);
		lStatus = PdhCollectQueryData(hQuery);

		if (lStatus != ERROR_SUCCESS)
		{
			return;
		}

		// 句柄总数
		lStatus = PdhGetFormattedCounterValue(hcSysHandleCount, PDH_FMT_LONG, NULL, &cv);

		if (lStatus == ERROR_SUCCESS)
		{
			m_nSysHandleCount = cv.longValue;
		}
		// 线程总数
		lStatus = PdhGetFormattedCounterValue(hcSysThreads, PDH_FMT_LONG, NULL, &cv);

		if (lStatus == ERROR_SUCCESS)
		{
			m_nSysThreadCount = cv.longValue;

		}
		// 进程总数
		lStatus = PdhGetFormattedCounterValue(hcSysProcesses, PDH_FMT_LONG, NULL, &cv);

		if (lStatus == ERROR_SUCCESS)
		{
			m_nSysProcessCount = cv.longValue;
		}
		lStatus = PdhGetFormattedCounterValue(hcSysCpu, PDH_FMT_LARGE, NULL, &cv);

		if (lStatus == ERROR_SUCCESS)
		{
			m_nSysCpu = cv.largeValue;
		}
		lStatus = PdhGetFormattedCounterValue(hcSysAvailableMem, PDH_FMT_LARGE, NULL, &cv);

		if (lStatus == ERROR_SUCCESS)
		{
			m_lSysAvailableMem = cv.largeValue / 1024;
		}

		// 核心内存：分页数
		lStatus = PdhGetFormattedCounterValue(hcKernelPaged, PDH_FMT_LARGE, NULL, &cv);
		if (lStatus == ERROR_SUCCESS)
		{
			m_lCoreMem1 = cv.largeValue / 1024;
		}
		// 核心内存：未分页数
		lStatus = PdhGetFormattedCounterValue(hcKernelNonpaged, PDH_FMT_LARGE, NULL, &cv);
		if (lStatus == ERROR_SUCCESS)
		{
			m_lCoreMem2 = cv.largeValue / 1024;
		}
		// 认可用量：总数
		lStatus = PdhGetFormattedCounterValue(hcCommitTotal, PDH_FMT_LARGE, NULL, &cv);
		if (lStatus == ERROR_SUCCESS)
		{
			m_lCommitMem1 = cv.largeValue / 1024;
		}
		// 认可用量：限制
		lStatus = PdhGetFormattedCounterValue(hcCommitLimit, PDH_FMT_LARGE, NULL, &cv);
		if (lStatus == ERROR_SUCCESS)
		{
			m_lCommitMem2 = cv.largeValue / 1024;
		}
		PdhRemoveCounter(hcCommitTotal);
		PdhRemoveCounter(hcCommitLimit);
		PdhRemoveCounter(hcKernelPaged);
		PdhRemoveCounter(hcKernelNonpaged);
		PdhRemoveCounter(hcSysAvailableMem);
		PdhRemoveCounter(hcSysCpu);
		PdhRemoveCounter(hcSysHandleCount);
		PdhRemoveCounter(hcSysProcesses);
		PdhRemoveCounter(hcSysThreads);
		PdhCloseQuery(hQuery);
	}
	else
		if (Mode == 2)
		{
			HQUERY hQuery;
			HCOUNTER hcCommitTotal, hcCommitLimit;
			PDH_STATUS lStatus = PdhOpenQuery(NULL, NULL, &hQuery);

			if (lStatus != ERROR_SUCCESS)
			{
				return;
			}
			PdhAddCounter(hQuery, _T("\\Memory\\Committed Bytes"), NULL, &hcCommitTotal);
			PdhAddCounter(hQuery, _T("\\Memory\\Commit Limit"), NULL, &hcCommitLimit);

			PDH_FMT_COUNTERVALUE cv;

			memset(&cv, 0, sizeof(PDH_FMT_COUNTERVALUE));

			lStatus = PdhCollectQueryData(hQuery);

			if (lStatus != ERROR_SUCCESS)
			{
				return;
			}
			// 认可用量：总数
			lStatus = PdhGetFormattedCounterValue(hcCommitTotal, PDH_FMT_LARGE, NULL, &cv);
			if (lStatus == ERROR_SUCCESS)
			{
				m_lCommitMem1 = cv.largeValue / 1024;
			}
			// 认可用量：限制
			lStatus = PdhGetFormattedCounterValue(hcCommitLimit, PDH_FMT_LARGE, NULL, &cv);
			if (lStatus == ERROR_SUCCESS)
			{
				m_lCommitMem2 = cv.largeValue / 1024;
			}
			PdhRemoveCounter(hcCommitTotal);
			PdhRemoveCounter(hcCommitLimit);
			PdhCloseQuery(hQuery);
		}
	RedrawWindow(hwnd, NULL, NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
};





void OnCreate(HWND hwnd) {
	//设置窗口扩展风格
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | 0x80000);
	typedef BOOL(WINAPI *FSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
	FSetLayeredWindowAttributes SetLayeredWindowAttributes;
	HINSTANCE hInst = LoadLibrary("User32.DLL");
	SetLayeredWindowAttributes = (FSetLayeredWindowAttributes)GetProcAddress(hInst, "SetLayeredWindowAttributes");
	if (SetLayeredWindowAttributes)
		SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 175, 2);//这里设置透明度和透明方式1,2两种
	FreeLibrary(hInst);
}

void OnPaint(HWND hwnd) {
	PAINTSTRUCT ps = { 0 };
	HDC hdc = BeginPaint(hwnd, &ps);
	switch (Mode)
	{
	case 0:
	{
		HFONT h = CreateFont(18, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "微软雅黑");
		SelectObject(hdc, h);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		CString str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11;
		if(!mb2)
		    str1.Format("下载速度: %u KB/S", dwBandIn);
		else
			str1.Format("下载速度: %.2lf MB/S", mb1);
		if(!mb1)
		    str2.Format("上传速度: %u KB/S", dwBandOut);
		else
			str2.Format("上传速度: %.2lf MB/S", mb2);
		str3.Format("句柄总数: %u ", m_nSysHandleCount);
		str4.Format("线程总数: %u ", m_nSysThreadCount);
		str5.Format("进程总数: %u ", m_nSysProcessCount);
		str6.Format("CPU利用率: %u%%", m_nSysCpu);
		str7.Format("可用内存: %u KB", m_lSysAvailableMem);
		str8.Format("核心内存(分页): %u KB", m_lCoreMem1);
		str9.Format("核心内存(未分页): %u KB", m_lCoreMem2);
		str10.Format("内存总量: %u KB", m_lCommitMem1);
		str11.Format("内存上限: %u KB", m_lCommitMem2);
		TextOut(hdc, 5, 20, (LPCSTR)str1, str1.GetLength());
		TextOut(hdc, 5, 0, (LPCSTR)str2, str2.GetLength());
		TextOut(hdc, 5, 40, (LPCSTR)str3, str3.GetLength());
		TextOut(hdc, 5, 60, (LPCSTR)str4, str4.GetLength());
		TextOut(hdc, 5, 80, (LPCSTR)str5, str5.GetLength());
		TextOut(hdc, 5, 100, (LPCSTR)str6, str6.GetLength());
		TextOut(hdc, 5, 120, (LPCSTR)str7, str7.GetLength());
		TextOut(hdc, 5, 140, (LPCSTR)str8, str8.GetLength());
		TextOut(hdc, 5, 160, (LPCSTR)str9, str9.GetLength());
		TextOut(hdc, 5, 180, (LPCSTR)str10, str10.GetLength());
		TextOut(hdc, 5, 200, (LPCSTR)str11, str11.GetLength());
		DeleteObject(h);
		HBRUSH hb;
		hb = (HBRUSH)GetStockObject(WHITE_BRUSH);
		SelectObject(hdc, hb);
		RECT r1;
		r1.top = 200;
		r1.left = 165;
		r1.bottom = 220;
		r1.right = 170;
		FillRect(hdc, &r1, hb);
		r1.top = 215;
		r1.left = 150;
		r1.right = 170;
		r1.bottom = 220;
		FillRect(hdc, &r1, hb);
		DeleteObject(hb);
	}
		break;
	case 2:
	{
		RECT r1;
		HBRUSH hb2;
		double per = ((double)m_lCommitMem1 / m_lCommitMem2);
		if(per<0.8)
		   hb2 = (HBRUSH)CreateSolidBrush(RGB(1, 77, 103));
		else
			hb2= (HBRUSH)CreateSolidBrush(RGB(251, 178, 23));
		SelectObject(hdc, hb2);
		r1.top = 0;
		r1.left = 0;
		r1.bottom = HEIGHT2;
		r1.right = WIDTH2 * per;
		FillRect(hdc, &r1, hb2);
		DeleteObject(hb2);
		HFONT h = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "微软雅黑");
		SelectObject(hdc, h);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		CString str1, str2;
		if (!mb1)
			str1.Format("下载: %u KB/S", dwBandIn);
		else
			str1.Format("下载: %.2lf MB/S", mb1);
		if (!mb2)
			str2.Format("上传: %u KB/S", dwBandOut);
		else
			str2.Format("上传: %.2lf MB/S", mb2);
		TextOut(hdc, 0, 20, (LPCSTR)str1, str1.GetLength());
		TextOut(hdc, 0, 0, (LPCSTR)str2, str2.GetLength());
		DeleteObject(h);
		HBRUSH hb;
		hb = (HBRUSH)GetStockObject(WHITE_BRUSH);
		SelectObject(hdc, hb);
		r1.top = 30;
		r1.left = 85;
		r1.bottom = 40;
		r1.right = 90;
		FillRect(hdc, &r1, hb);
		r1.top = 35;
		r1.left = 80;
		r1.right = 90;
		r1.bottom = 40;
		FillRect(hdc, &r1, hb);
		DeleteObject(hb);
	}
		break;
	}
	EndPaint(hwnd, &ps);



}

void OnLbuttonDown(HWND hwnd, LPARAM lParam) {
	//无边框窗口的移动
	PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}



BOOL Register(WNDPROC fWndProc, HINSTANCE hInstance, LPCTSTR szClassName) {
	WNDCLASSEX wce = { 0 };
	wce.cbSize = sizeof(wce);
	wce.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wce.lpfnWndProc = fWndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = hInstance;
	wce.hIcon = NULL;
	wce.hCursor = LoadCursor(NULL,IDC_ARROW);
	wce.hbrBackground = CreateSolidBrush(RGB(96, 143, 159));
	wce.lpszMenuName = NULL;
	wce.lpszClassName = szClassName;
	wce.hIconSm = NULL;
	ATOM nAtom = RegisterClassEx(&wce);
	if (nAtom == 0) return false;
	return true;
}
BOOL Register2(WNDPROC fWndProc, HINSTANCE hInstance, LPCTSTR szClassName) {
	WNDCLASSEX wce = { 0 };
	wce.cbSize = sizeof(wce);
	wce.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wce.lpfnWndProc = fWndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = hInstance;
	wce.hIcon = NULL;
	wce.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce.hbrBackground = CreateSolidBrush(WHITE_BRUSH);
	wce.lpszMenuName = NULL;
	wce.lpszClassName = szClassName;
	wce.hIconSm = NULL;
	ATOM nAtom = RegisterClassEx(&wce);
	if (nAtom == 0) return false;
	return true;
}

HWND Create(LPCTSTR lpClassName, LPCTSTR lpWindowName, HINSTANCE hInstance) {

	return CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,
		lpClassName, lpWindowName, WS_POPUP | WS_SYSMENU,
		SCREEN_WIDTH*0.8, 0, WIDTH2, HEIGHT2,
		NULL, NULL, hInstance, NULL);

}


HWND Create2(LPCTSTR lpClassName, LPCTSTR lpWindowName, HINSTANCE hInstance,int x,int y) {

	return CreateWindowEx(WS_EX_TOOLWINDOW,
		lpClassName, lpWindowName, WS_POPUP | WS_SYSMENU,
		x, y, CHILDWIDTH, CHILDHEIGHT,
		NULL, NULL, hInstance, NULL);

}

HWND Create3(LPCTSTR lpClassName, LPCTSTR lpWindowName, HINSTANCE hInstance, HWND parent) {

	RECT rect;
	GetWindowRect(parent, &rect);
	return CreateWindowEx(WS_EX_TOOLWINDOW,
		lpClassName, lpWindowName, WS_POPUP | WS_SYSMENU,
		rect.left, rect.top-CHILDHEIGHT, CHILDWIDTH, CHILDHEIGHT,
		parent, NULL, hInstance, NULL);

}

HWND Create4(LPCTSTR lpClassName, LPCTSTR lpWindowName, HINSTANCE hInstance, HWND parent) {

	RECT rect;
	GetWindowRect(parent, &rect);
	return CreateWindowEx(WS_EX_TOOLWINDOW,
		lpClassName, lpWindowName, WS_POPUP | WS_SYSMENU,
		rect.right-CHILDWIDTH2, rect.bottom, CHILDWIDTH2, CHILDHEIGHT2,
		parent, NULL, hInstance, NULL);
}


void Display(HWND hwnd) {
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void Message() {
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}




HWND CreateTagWindow(HWND hwnd, char* string1,int x=0,int y=0)
{
	HINSTANCE hinstance;
	hinstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	HWND hwnd1 = Create2("Text", NULL, hinstance, x?x:rand() % SCREEN_WIDTH*0.4-CHILDWIDTH-20+SCREEN_WIDTH*0.6, y?y:rand() % SCREEN_HEIGHT*0.6+SCREEN_HEIGHT*0.2);
	CString str2 = string1;
	pair<HWND, CString>ab(hwnd1, str2);
	combos.insert(ab);
	Display(hwnd1);
	RedrawWindow(hwnd1, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	return hwnd1;
}



LRESULT CALLBACK WindowProc4(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_CREATE:
		OnCreate(hwnd);
		SetTimer(hwnd, Timercount, 1000, TimerProc6);
		Timercount++;
		SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)colors[((int)GetParent(hwnd) / 10) % 10]);
		AnimateWindow(hwnd, 100, AW_SLIDE | AW_VER_POSITIVE);
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		combohwnd2.erase(GetParent(hwnd));
		combospan.erase(GetParent(hwnd));
		SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)colors[((int)GetParent(hwnd) / 10) % 10]);
		AnimateWindow(hwnd, 100, AW_HIDE | AW_VER_NEGATIVE);
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hwnd, &ps);
		HFONT h = CreateFont(20, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "微软雅黑");
		SelectObject(hdc, h);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		CString string1,str2,str3,str4,fstr;
		CTimeSpan t = combospan.find(GetParent(hwnd))->second;
		RECT rect;
		GetClientRect(hwnd, &rect);
		string1.Format("%lld天", t.GetDays());
		if(t.GetHours()>=10)
		    str2.Format("%ld:", t.GetHours());
		else
			str2.Format("0%ld:", t.GetHours());
		if (t.GetMinutes() >= 10)
			str3.Format("%ld:", t.GetMinutes());
		else
			str3.Format("0%ld:", t.GetMinutes());
		if (t.GetSeconds() >= 10)
			str4.Format("%ld", t.GetSeconds());
		else
			str4.Format("0%ld", t.GetSeconds());
		fstr = string1 + str2 + str3 + str4;
		DrawText(hdc, (LPCSTR)fstr, fstr.GetLength(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hwnd, &ps);
	}
	break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}




LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_CREATE:
		OnCreate(hwnd);
		SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)colors[((int)hwnd / 10) % 10]);
		AnimateWindow(hwnd, 100, AW_SLIDE | AW_CENTER);
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);
		if(warn.find(hwnd)==warn.end())
		   FillRect(hdc, &rect, colors[((int)hwnd / 10) % 10]);
		else
		{
			
			int a = warn.find(hwnd)->second;
			if (a < 100)
			{
				HBRUSH br = CreateSolidBrush(RGB(255, a*1.5, a*1.5));
				HGDIOBJ old=SelectObject(hdc, br);
				FillRect(hdc, &rect, br);
				SelectObject(hdc,old);
				DeleteObject(br);
			}
			else
			{
				HBRUSH br = CreateSolidBrush(RGB(255, (200-a)*1.5, (200-a)*1.5));
				HGDIOBJ old = SelectObject(hdc, br);
				FillRect(hdc, &rect, br);
				SelectObject(hdc, old);
				DeleteObject(br);
			}
			
		}
		if (focuson == hwnd)
		{	
			HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			rect.top = 0;
			rect.left = 0;
			rect.bottom = 2;
			rect.right = 10;
			FillRect(hdc, &rect, brush);
			rect.top = 0;
			rect.left = 0;
			rect.bottom = 10;
			rect.right = 2;
			FillRect(hdc, &rect, brush);
			rect.top = CHILDHEIGHT-10;
			rect.left = 0;
			rect.bottom = CHILDHEIGHT;
			rect.right = 2;
			FillRect(hdc, &rect, brush);
			rect.top = CHILDHEIGHT;
			rect.left = 0;
			rect.bottom = CHILDHEIGHT-2;
			rect.right = 10;
			FillRect(hdc, &rect, brush);
			rect.top = 0;
			rect.left = CHILDWIDTH-10;
			rect.bottom = 2;
			rect.right = CHILDWIDTH;
			FillRect(hdc, &rect, brush);
			rect.top = 0;
			rect.left = CHILDWIDTH;
			rect.bottom = 10;
			rect.right = CHILDWIDTH-2;
			FillRect(hdc, &rect, brush);
			rect.top = CHILDHEIGHT-10;
			rect.left = CHILDWIDTH-2;
			rect.bottom = CHILDHEIGHT;
			rect.right = CHILDWIDTH;
			FillRect(hdc, &rect, brush);
			rect.top = CHILDHEIGHT - 2;
			rect.left = CHILDWIDTH - 10;
			rect.bottom = CHILDHEIGHT;
			rect.right = CHILDWIDTH;
			FillRect(hdc, &rect, brush);
		}
		GetClientRect(hwnd, &rect);
		HFONT h = CreateFont(30, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "微软雅黑");
		HGDIOBJ old=SelectObject(hdc, h);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		CString string1 = combos.find(hwnd)->second;
		DrawText(hdc, (LPCSTR)string1, string1.GetLength(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		SelectObject(hdc, old);
		DeleteObject(h);
		EndPaint(hwnd, &ps);
	}
		break;
	case WM_LBUTTONDOWN:
	{
		if (combohwnd.find(hwnd) != combohwnd.end())
		{
			SendMessage(combohwnd.find(hwnd)->second, WM_DESTROY, 0, 0);
			combohwnd.erase(hwnd);
		}
		OnLbuttonDown(hwnd, lParam);
		if (focuson)
		{
			focuson = NULL;
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}
	}
	    break;
	case WM_MOVE:
	case WM_MOVING:
		if (combohwnd2.find(hwnd) != combohwnd2.end())
		{
			RECT rect;
			GetWindowRect(hwnd, &rect);
			MoveWindow(combohwnd2.find(hwnd)->second, rect.right - CHILDWIDTH2, rect.bottom, CHILDWIDTH2, CHILDHEIGHT2, false);
			//OnLbuttonDown(combohwnd2.find(hwnd)->second, lParam);
		}
		break;
	case WM_RBUTTONDBLCLK:
		break;
	case WM_KILLFOCUS:
	{
		if (!focuson)
		{
			PostMessage(combohwnd.find(hwnd)->second, WM_DESTROY, 0, 0);
			combohwnd.erase(hwnd);
		}
		focuson = NULL;
		char szAppPath[MAX_PATH];
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0;
		CString stra = szAppPath;
		stra += "\\data.dat";
		FILE * fp;
		fopen_s(&fp, stra, "w+");
		if (fp != NULL)
		{
			map<HWND, CString>::iterator it;
			for (it = combos.begin();it != combos.end();++it)
				if (it->second != "\n"&& it->second != "\r\n"&& it->second != "")
				{
					RECT rect;
					GetWindowRect(it->first, &rect);
					if (combotime.find(it->first) == combotime.end())
						fprintf(fp, "%s$%d %d\r\n", it->second.GetBuffer(0), rect.left, rect.top);
					else
					{
						Time *t = combotime.find(it->first)->second;
						fprintf(fp, "%s$%d %d %d %d %d %d %d\r\n", it->second.GetBuffer(0), rect.left, rect.top, t->year, t->month, t->day, t->hour, t->minute);
					}
				}
		}
		fclose(fp);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	}
		break;
	case WM_MOUSEMOVE:
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		if (rect.right > SCREEN_WIDTH || rect.bottom > SCREEN_HEIGHT)
		{
			if (rect.right > SCREEN_WIDTH)
				rect.left = SCREEN_WIDTH - width;
			else
				rect.top = SCREEN_HEIGHT - height;
			MoveWindow(hwnd, rect.left, rect.top, width, height, false);
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		if(focuson==hwnd)
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (zDelta == 120)
			{
				if (combohwnd.find(hwnd) == combohwnd.end())
				{
					if (combohwnd2.find(hwnd) != combohwnd2.end())
					{
						DestroyWindow(combohwnd2.find(hwnd)->second);
						SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
					}
					//如果父窗口已经有timer kill掉
					if (combotimer.find(hwnd) != combotimer.end())
					{
						KillTimer(hwnd, combotimer.find(hwnd)->second);
						combotimer.erase(hwnd);
					}
					TimerChanged = true;
					//如果没有定时 增加定时 如果有 直接输出
					if (combotime.find(hwnd) == combotime.end())
					{
						CTime timenow = CTime::GetCurrentTime();
						Time *t = new Time();
						t->year = timenow.GetYear();
						t->month = timenow.GetMonth();
						t->day = timenow.GetDay();
						t->hour = timenow.GetHour();
						t->minute = timenow.GetMinute();
						pair<HWND, Time*> pt(hwnd, t);
						combotime.insert(pt);
						TimerChanged = false;
					}
					HINSTANCE hinstance;
					hinstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
					HWND hwnd1 = Create3("Time", NULL, hinstance, hwnd);
					pair<HWND, HWND> p(hwnd, hwnd1);
					combohwnd.insert(p);
					SendMessage(hwnd1, WM_LBUTTONDOWN, 0, 0);
				}
			}
			else
				if (zDelta == -120)
				{
					if (combotime.find(hwnd) != combotime.end() && combohwnd2.find(hwnd) == combohwnd2.end())
					{
						SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
						Time *t=combotime.find(hwnd)->second;
						CTime timenow = CTime::GetCurrentTime();
						CTime ct(t->year, t->month, t->day, t->hour, t->minute, 0);
						if (ct > timenow)
						{
							CTimeSpan tmSpan = ct - timenow;
							combospan.insert(pair<HWND, CTimeSpan>(hwnd, tmSpan));
							HINSTANCE hinstance;
							hinstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
							HWND hwnd1 = Create4("TimeSpan", NULL, hinstance, hwnd);
							pair<HWND, HWND> p(hwnd, hwnd1);
							combohwnd2.insert(p);
						}
					}
				}
		}
	}
		break;
	case WM_RBUTTONDOWN:
	{
		focuson = hwnd;
		map<HWND, CString>::iterator it;
		for (it = combos.begin();it != combos.end();++it)
			RedrawWindow(it->first, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	}
		break;
	case WM_CHAR:
		if (focuson == hwnd)
		{
			if (wParam != 8)
				combos.find(hwnd)->second.AppendChar(wParam);
			else
			{
				int a = combos.find(hwnd)->second.GetLength();
				if(a>=2 && IsDBCSLeadByte(combos.find(hwnd)->second.GetAt(a-2)))
				    combos.find(hwnd)->second.Delete(a - 2,2);
				else
					combos.find(hwnd)->second.Delete(a - 1);
			}
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}
		break;
	case WM_DESTROY:
	{
		combos.erase(hwnd);
		if (combotime.find(hwnd) != combotime.end())
		{
			delete combotime.find(hwnd)->second;
			combotime.find(hwnd)->second = NULL;
			combotime.erase(hwnd);
		}
		char szAppPath[MAX_PATH];
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0;
		CString stra = szAppPath;
		stra += "\\data.dat";
		FILE * fp;
		fopen_s(&fp, stra, "w+");
		if (fp != NULL)
		{
			map<HWND, CString>::iterator it;
			for (it = combos.begin();it != combos.end();++it)
				if (it->second != "\n"&& it->second != "\r\n"&& it->second != "")
				{
					RECT rect;
					GetWindowRect(it->first, &rect);
					if (combotime.find(it->first) == combotime.end())
						fprintf(fp, "%s$%d %d\r\n", it->second.GetBuffer(0), rect.left, rect.top);
					else
					{
						Time *t = combotime.find(it->first)->second;
						fprintf(fp, "%s$%d %d %d %d %d %d %d\r\n", it->second.GetBuffer(0), rect.left, rect.top, t->year, t->month, t->day, t->hour, t->minute);
					}
				}
		}
		fclose(fp);
	}
		break;
	case WM_LBUTTONDBLCLK:
		SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)colors[((int)hwnd / 10) % 10]);
		AnimateWindow(hwnd, 200, AW_HIDE | AW_CENTER);
		DestroyWindow(hwnd);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}





LRESULT CALLBACK WindowProc3(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_CREATE:
		OnCreate(hwnd);
		SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)colors[((int)GetParent(hwnd) / 10) % 10]);
		AnimateWindow(hwnd, 100, AW_SLIDE | AW_VER_NEGATIVE);
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_DESTROY:
	{
		//给父窗口一个Timer 时间到了就触发
		//计算需要计的时间,若超过USER_TIMER_MAXIMUM则限制
		if (TimerChanged)
		{
			warn.erase(GetParent(hwnd));
			SetWindowPos(GetParent(hwnd), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
			LONGLONG sec = GetTimerSecond(GetParent(hwnd));
			if (sec < 1000)
			{
				SetWindowPos(GetParent(hwnd), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				pair<HWND, int> p(GetParent(hwnd), 0);
				warn.insert(p);
				SetTimer(GetParent(hwnd), Timercount, 50, TimerProc5);
			}
			else
			{
				if (sec >= USER_TIMER_MAXIMUM)
					SetTimer(GetParent(hwnd), Timercount, USER_TIMER_MAXIMUM, TimerProc4);
				else
					SetTimer(GetParent(hwnd), Timercount, sec, TimerProc4);
			}
			pair<HWND, UINT_PTR> p(GetParent(hwnd), Timercount);
			combotimer.insert(p);
			Timercount++;
			TimerChanged = false;
			RedrawWindow(GetParent(hwnd), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}
		else
		{
			combotime.erase(GetParent(hwnd));
		}
		SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)colors[((int)GetParent(hwnd) / 10) % 10]);
		AnimateWindow(hwnd, 100, AW_HIDE | AW_VER_POSITIVE);
	}
		break;
	case WM_LBUTTONDOWN:
		if(focuson)
		   OnLbuttonDown(hwnd, lParam);
		break;
	case WM_MOUSEWHEEL:
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(hwnd, &pos);
		Time *t = combotime.find(GetParent(hwnd))->second;
		if (pos.y > 0 && pos.y < 30)
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (zDelta == 120)
			{
				if (pos.x > 15 && pos.x < 70)
				{
					t->year++;
					if (t->year > 9999)
					{
						CTime timenow = CTime::GetCurrentTime();
						t->year = timenow.GetYear();
					}
				}
				else
					if (pos.x > 90 && pos.x < 120)
					{
						t->month++;
						if (t->month > 12)
							t->month = 1;
						int limit;
						if (t->month == 2)
							if ((t->year % 4 == 0 && t->year % 100 != 0) || t->year % 400 == 0)
								limit = 29;
							else
								limit = 28;
						else
							limit = monthday[t->month - 1];
						if (t->day > limit)
							t->day = limit;
					}
					else
						if (pos.x > 140 && pos.x < 170)
						{
							t->day++;
							int limit;
							if (t->month == 2)
								if ((t->year % 4 == 0 && t->year % 100 != 0) || t->year % 400 == 0)
									limit = 29;
								else
									limit = 28;
							else
								limit = monthday[t->month - 1];
							if (t->day > limit)
								t->day = 1;
						}
						else
							if (pos.x > 190 && pos.x < 220)
							{
								t->hour++;
								if (t->hour > 23)
									t->hour = 0;
							}
							else
								if (pos.x > 240 && pos.x < 270)
								{
									t->minute++;
									if (t->minute > 59)
										t->minute = 0;
								}
			}
			if (zDelta == -120)
			{
				if (pos.x > 15 && pos.x < 70)
				{
					t->year--;
					CTime timenow = CTime::GetCurrentTime();
					if (t->year < timenow.GetYear())
						t->year = timenow.GetYear();
				}
				else
					if (pos.x > 90 && pos.x < 120)
					{
						t->month--;
						if (t->month < 1)
							t->month = 12;
						int limit;
						if (t->month == 2)
							if ((t->year % 4 == 0 && t->year % 100 != 0) || t->year % 400 == 0)
								limit = 29;
							else
								limit = 28;
						else
							limit = monthday[t->month - 1];
						if (t->day > limit)
							t->day = limit;
					}
					else
						if (pos.x > 140 && pos.x < 170)
						{
							t->day--;
							int limit;
							if (t->month == 2)
								if ((t->year % 4 == 0 && t->year % 100 != 0) || t->year % 400 == 0)
									limit = 29;
								else
									limit = 28;
							else
								limit = monthday[t->month - 1];
							if (t->day < 1)
								t->day = limit;
						}
						else
							if (pos.x > 190 && pos.x < 220)
							{
								t->hour--;
								if (t->hour < 0)
									t->hour = 23;
							}
							else
								if (pos.x > 240 && pos.x < 270)
								{
									t->minute--;
									if (t->minute<0)
										t->minute = 59;
								}
			}
			TimerChanged = true;
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		}
	}
		break;
	case WM_SETFOCUS:
		break;
	case WM_KILLFOCUS:
		SendMessage(GetParent(hwnd), WM_KILLFOCUS, 0, 0);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, colors[((int)GetParent(hwnd) / 10) % 10]);
		GetClientRect(hwnd, &rect);
		HFONT h1 = CreateFont(18, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "微软雅黑");
		SelectObject(hdc, h1);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 70, 12, "年", 2);
		TextOut(hdc, 120, 12, "月", 2);
		TextOut(hdc, 170, 12, "日", 2);
		TextOut(hdc, 220, 12, "时", 2);
		TextOut(hdc, 270, 12, "分", 2);
		DeleteObject(h1);
		HFONT h = CreateFont(30, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "微软雅黑");
		SelectObject(hdc, h);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		CString str[5];
		Time *t = combotime.find(GetParent(hwnd))->second;
		str[0].Format("%d", t->year);
		if(t->month>=10)
		    str[1].Format("%d", t->month);
		else
			str[1].Format("0%d", t->month);
		if (t->day >= 10)
		    str[2].Format("%d", t->day);
		else
			str[2].Format("0%d", t->day);
		if (t->hour >= 10)
			str[3].Format("%d", t->hour);
		else
			str[3].Format("0%d", t->hour);
		if (t->minute >= 10)
			str[4].Format("%d", t->minute);
		else
			str[4].Format("0%d", t->minute);
		TextOut(hdc, 15, 5, str[0], str[0].GetLength());
		TextOut(hdc, 90, 5, str[1], str[1].GetLength());
		TextOut(hdc, 140, 5, str[2], str[2].GetLength());
		TextOut(hdc, 190, 5, str[3], str[3].GetLength());
		TextOut(hdc, 240, 5, str[4], str[4].GetLength());
		DeleteObject(h);
		EndPaint(hwnd, &ps);
	}
	break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
	{
		for (int i = 0;i < 11;i++)
			DeleteObject(colors[i]);
		char szAppPath[MAX_PATH];
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0;
		CString stra = szAppPath;
		stra += "\\data.dat";
		FILE * fp;
		fopen_s(&fp, stra, "w+");
		if (fp != NULL)
		{
			map<HWND, CString>::iterator it;
			for (it = combos.begin();it != combos.end();++it)
				if (it->second != "\n"&& it->second != "\r\n"&& it->second != "")
				{
					RECT rect;
					GetWindowRect(it->first, &rect);
					if(combotime.find(it->first)==combotime.end())
					    fprintf(fp, "%s$%d %d\r\n", it->second.GetBuffer(0),rect.left,rect.top);
					else
					{
						Time *t = combotime.find(it->first)->second;
						fprintf(fp, "%s$%d %d %d %d %d %d %d\r\n", it->second.GetBuffer(0), rect.left, rect.top,t->year,t->month,t->day,t->hour,t->minute);
					}
				}
		}
		fclose(fp);
		PostQuitMessage(100);
	}
		break;
	case WM_CREATE:
	{
		if (uRetCode == ERROR_INSUFFICIENT_BUFFER)
		{
			m_pTable = (PMIB_IFTABLE)new BYTE[65535];
		}
		SetTimer(hwnd, 1, 1000, TimerProc);
		char szAppPath[MAX_PATH];
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0;
		CString stra = szAppPath;
		stra += "\\data.dat";
		FILE * fp;
		fopen_s(&fp, stra, "r");
		if (fp != NULL)
		{
			while (!feof(fp))
			{
				char buff[80]="";
				fgets(buff, 79, fp);
				StrReplaceB(buff, "\r", "");
				StrReplaceB(buff, "\n", "");
				if (buff[0])
				{
					char *scliced;
					_strrev(buff);
					strtok_s(buff, "$", &scliced);
					_strrev(scliced);
					if (countstr(buff, " ") < 4)
					{
						char *buff2;
						strtok_s(buff, " ", &buff2);
						_strrev(buff);
						_strrev(buff2);
						CreateTagWindow(hwnd, scliced, atoi(buff2), atoi(buff));
					}
					else
					{
						char *buff2, *buff3,*buff4,*buff5,*buff6,*buff7,*buff8;
						buff2 = strtok_s(buff, " ", &buff3);
						buff2 = strtok_s(NULL, " ", &buff3);
						buff4 = strtok_s(NULL, " ", &buff3);
						buff5 = strtok_s(NULL, " ", &buff3);
						buff6 = strtok_s(NULL, " ", &buff3);
						buff7 = strtok_s(NULL, " ", &buff3);
						buff8 = strtok_s(NULL, " ", &buff3);
						_strrev(buff);
						_strrev(buff2);
						_strrev(buff4);
						_strrev(buff5);
						_strrev(buff6);
						_strrev(buff7);
						_strrev(buff8);
						HWND hwnd1=CreateTagWindow(hwnd, scliced, atoi(buff8), atoi(buff7));
						Time *t = new Time();
						t->year = atoi(buff6);
						t->month = atoi(buff5);
						t->day = atoi(buff4);
						t->hour = atoi(buff2);
						t->minute = atoi(buff);
						combotime.insert(pair<HWND, Time*>(hwnd1, t));
						LONGLONG sec = GetTimerSecond(hwnd1);
						if (sec < 1000)
						{
							SetWindowPos(hwnd1, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
							pair<HWND, int> p(hwnd1, 0);
							warn.insert(p);
							SetTimer(hwnd1, Timercount, 50, TimerProc5);
						}
						else
						{
							if (sec >= USER_TIMER_MAXIMUM)
								SetTimer(hwnd1, Timercount, USER_TIMER_MAXIMUM, TimerProc4);
							else
								SetTimer(hwnd1, Timercount, sec, TimerProc4);
						}
						pair<HWND, UINT_PTR> p(hwnd1, Timercount);
						combotimer.insert(p);
						Timercount++;
						RedrawWindow(hwnd1, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
					}
				}
			}
			fclose(fp);
		}
		OnCreate(hwnd);
		AnimateWindow(hwnd, 100, AW_SLIDE | AW_CENTER);
		InvalidateRect(hwnd, NULL, FALSE);
	}
		break;
	case WM_PAINT:
		OnPaint(hwnd);
		break;
	case WM_LBUTTONDOWN:
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(hwnd, &pos);
		switch (Mode)
		{
		case 0:
			if (pos.x > 150 && pos.y > 200)
			{
				SetTimer(hwnd, 2, 10, TimerProc2);
				Mode++;
			}
			else
				OnLbuttonDown(hwnd, lParam);
			break;
		case 2:
			if (pos.x > 70 && pos.y > 30)
			{
				SetTimer(hwnd, 3, 10, TimerProc3);
				Mode--;
			}
			else
				OnLbuttonDown(hwnd, lParam);
			break;
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		if (rect.right > SCREEN_WIDTH || rect.bottom > SCREEN_HEIGHT)
		{
			if (rect.right > SCREEN_WIDTH)
				rect.left = SCREEN_WIDTH - width;
			else
				rect.top = SCREEN_HEIGHT - height;
			MoveWindow(hwnd, rect.left, rect.top, width, height, false);
		}
	}
	break;
	case WM_LBUTTONDBLCLK:
		AnimateWindow(hwnd, 200, AW_HIDE | AW_CENTER);
		DestroyWindow(hwnd);
		KillTimer(hwnd, 1);
		break;
	case WM_RBUTTONDOWN:
	{
		CreateTagWindow(hwnd, "");
	}
	break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	HWND mainhwnd = FindWindow("_(:3 」∠)_", "StatsWindow");
	if (!mainhwnd)
	{
		g_hInstance = hInstance;
		srand((unsigned int)time(NULL));
		Register(WindowProc, hInstance, "_(:3 」∠)_");
		Register2(WindowProc2, hInstance, "Text");
		Register2(WindowProc3, hInstance, "Time");
		Register2(WindowProc4, hInstance, "TimeSpan");
		HWND hwnd = Create("_(:3 」∠)_", "StatsWindow", hInstance);
		Display(hwnd);
		Message();
	}
	else
		MessageBox(NULL, "已经存在一个主悬浮窗", "Error", MB_OK);
	return 0;
}



