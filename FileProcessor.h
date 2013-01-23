#include <set>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <boost\thread.hpp>
#include <boost\thread\mutex.hpp>
#include <boost\lexical_cast.hpp>

using namespace std;

#pragma once

class FileProcessor
{
public:
	FileProcessor(void);
	~FileProcessor(void);

	//external interface
	void fileAdd(wstring);
	int fileCount(); //returns amount of files in internal file list, 0 on empty list
	int filesClear(); //clears internal file list, returns amount of files removed from list
	const wstring dumpFileInfo();

private:
	int getFileInfo();
	wstring getFileSizeDate(WIN32_FILE_ATTRIBUTE_DATA);

	void workerCheckSum(queue< wstring > *); // worker function for thread pool
	unsigned short int maxThreads;
	vector< boost::thread > threadList; //list of threads
	boost::mutex csRead, csWrite;

	queue< wstring > queueCheckSum;
	set< wstring > fileList; //alphabetically sorted list of files
	map< wstring, wstring > fileDateSize; //information about files in fileList
	map< wstring, wstring > fileCheckSum; //map to get information from threads 

};