#include <set>
#include <string>
using namespace std;

#pragma once
class FileProcessor
{
public:
	FileProcessor(void);
	~FileProcessor(void);

	void addFile(wstring);
	const wstring dumpFileInfo();

private:
	set< wstring > fileList; //alphabetically sorted list of files
	wstring filePath;
};

