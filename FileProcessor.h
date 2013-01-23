#include <set>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <Shlwapi.h>

#include <boost\thread.hpp>
#include <boost\thread\mutex.hpp>

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


	void workerCheckSum(queue< wstring > *); // worker function for thread pool
	unsigned short int maxThreads;
	vector< boost::thread > threadList; //list of threads
	boost::mutex csRead, csWrite;

	queue< wstring > queueCheckSum;
	set< wstring > fileList; //alphabetically sorted list of files
	map< wstring, wstring > fileDateSize; //information about files in fileList
	map< wstring, wstring > fileCheckSum; //map to get information from threads 

};