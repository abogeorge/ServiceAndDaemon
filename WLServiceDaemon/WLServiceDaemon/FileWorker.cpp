#include "stdafx.h"
#include "FileWorker.h"

#ifdef _WIN32
char * path = "D:\\Projects\\ServiceAndDaemon\\Tests\\log.txt";
#elif linux
char * path = "//home//abo//Work//Projects//LinuxDaemon//Test//log.txt";
#endif

FileWorker::FileWorker()
{
}


FileWorker::~FileWorker()
{
}


void FileWorker::writeInit()
{
	ofstream out(path, ios::app);
	out.seekp(0, ios::end);
	out << "\n\n\n------------------------------------------------------\n";
	out << "Service Started: " << getTime();
	out << "\n------------------------------------------------------\n";
	out.close();
}


void FileWorker::writeContent()
{
	ofstream out(path, ios::app);
	out.seekp(0, ios::end);
	out << "\nStill runing: " << getTime();
	out.close();
	#ifdef _WIN32
	Sleep(1000);
	#elif linux
	sleep(1);
	#endif
}


void FileWorker::writeEnd()
{
	ofstream outf(path, ios::app);
	outf.seekp(0, ios::end);
	outf << "\n------------------------------------------------------\n";
	outf << "Service Stopped: " << getTime();
	outf << "\n------------------------------------------------------\n";
	outf.close();
}

std::string FileWorker::getTime(){
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	std::string str = string(buf);
	return str;
}