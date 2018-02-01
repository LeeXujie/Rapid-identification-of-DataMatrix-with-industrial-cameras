#include <Windows.h>
#include <stdio.h>
#include <conio.h>
class CTimer
{
public:
	static bool timer_flag;
	static UINT timeid;

	CTimer();
	~CTimer();
	void CloseTimer();
	void CreateTimerThread(int* time);
	static DWORD CALLBACK TimeThread(PVOID pvoid);
	static void CALLBACK TimeProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

private:
	
};

bool CTimer::timer_flag = false;
UINT CTimer::timeid = 0;


CTimer::CTimer()
{
	timer_flag = false;
	timeid = 0;
}

CTimer::~CTimer()
{
}

void CTimer::CreateTimerThread(int* time)
{
	HANDLE hand = CreateThread(NULL, 0, CTimer::TimeThread, time, 0, NULL);
}

void CTimer::CloseTimer()
{
	KillTimer(NULL, timeid);
}

DWORD CALLBACK CTimer::TimeThread(PVOID pvoid)
{
	int* time = (int*)pvoid;
	int itm = *time;
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
	timeid = SetTimer(NULL, 0, itm, CTimer::TimeProc);
	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			printf("Error:the thread will quit,error id is %d\n", GetLastError());
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	KillTimer(NULL, timeid);
	printf("thread end here\n");
	return 0;
}

void CALLBACK CTimer::TimeProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	//printf("WM_TIMER in work thread count\n");
	timer_flag = true;
}