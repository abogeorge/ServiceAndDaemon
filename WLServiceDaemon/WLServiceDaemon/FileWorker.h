#pragma once
class FileWorker
{
public:
	FileWorker();
	~FileWorker();
	void writeInit();
	void writeContent();
	void writeEnd();
	std::string getTime();
};

