#pragma once
class FileWorker
{
public:
	FileWorker();
	~FileWorker();
	void writeInit();
	void writeContent();
	void wirteEnd();
	std::string getTime();
};

