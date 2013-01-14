#include <set>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <process.h>
#include <Shlwapi.h>
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
	void getFileInfo();
	wstring getFileSizeDate(WIN32_FILE_ATTRIBUTE_DATA);
	static unsigned __stdcall getCheckSum(void*); 
	void saveCheckSum(wstring, DWORD); // method to save data from static thread


	set< wstring > fileList; //alphabetically sorted list of files
	map< wstring, wstring > fileDateSize; //information about files in fileList
	map< wstring, wstring > fileCheckSum; //map to get information from threads 

	vector< HANDLE > handleList; //list of threads
};

