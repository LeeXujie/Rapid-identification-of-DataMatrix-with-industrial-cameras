
// haikang64.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Chaikang64App: 
// �йش����ʵ�֣������ haikang64.cpp
//

class Chaikang64App : public CWinApp
{
public:
	Chaikang64App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Chaikang64App theApp;