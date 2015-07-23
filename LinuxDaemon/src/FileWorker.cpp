#include "stdafx.h"
#include "FileWorker.h"


FileWorker::FileWorker()
{
}


FileWorker::~FileWorker()
{
}

char * path = "//home//abo//Work//Projects//LinuxDaemon//Test//log.txt";

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
	//sleep(3000);
}


void FileWorker::wirteEnd()
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
