// WLServiceDaemon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	#ifdef _WIN32
	WindowsService wSvc;
	wSvc.ServiceEntry();
	#elif linux
	LinuxDaemon lDmn;
	lDmn.DaemonEntry();
	#endif
	return 0;
}

