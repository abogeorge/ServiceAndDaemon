#pragma once



class WindowsService
{
#ifdef _WIN32
public:
	WindowsService();
	~WindowsService();
	int ServiceEntry();
#endif
};


