#include "FileProcessor.h"


FileProcessor::FileProcessor(void)
{
	maxThreads = 10;
}


FileProcessor::~FileProcessor(void)
{
}

// interface to add files into internal filelist
void FileProcessor::addFile(wstring filename)
{
	fileList.insert(filename);
}

const wstring FileProcessor::dumpFileInfo()
{

	//constructing message with information about every file

	wstring message;
	message.append(L"File list(first 20 entries if too long to display):\n(File Name | Size | Creation Date | Simple Checksum)\n\n");
	getFileInfo();
	std::set<wstring>::iterator first = fileList.begin();
	for(std::set<wstring>::iterator i = fileList.begin(); distance(first,i) != 20 && i != fileList.end()  ; ++i)
	{
		wstring filename = *i;
		message.append(fileDateSize.at(filename))
		.append(fileCheckSum.at(filename))
		.append(L"\n");
	}

	return message;
}

void FileProcessor::getFileInfo()
{

	// Gathering date and size information about every file from WIN32_FILE_ATTRIBUTE_DATA structure

	for each (std::wstring filename in fileList)
	{
		wstring info;
		WIN32_FILE_ATTRIBUTE_DATA fileAttrData = {0};
		GetFileAttributesEx(filename.c_str(), GetFileExInfoStandard, &fileAttrData);

		//filename
		size_t delim = filename.find_last_of(L"\\");
		if( std::wstring::npos != delim )
			info = filename.substr(delim+1);
		else
			info = filename;

		info.append(L" | ").append(getFileSizeDate(fileAttrData));
		fileDateSize.insert( make_pair(filename, info) );

		//push file into thread pool queue
		queueCheckSum.push(filename);

	}

	//launch thread pool
	int threadCount = maxThreads;
	if( queueCheckSum.size() < maxThreads )
		threadCount = queueCheckSum.size();
	for(int i = 0; i < threadCount; i++){
		threadList.push_back(boost::thread(&FileProcessor::workerCheckSum, this, &queueCheckSum));
	}

	//waiting for threads to end
	for(int i = 0; i < threadCount; i++){
		threadList[i].join();
	}
}

wstring FileProcessor::getFileSizeDate(WIN32_FILE_ATTRIBUTE_DATA fileAttrData)
{
	wstringstream sstream;

	//file size
	wchar_t buffer[50];
	StrFormatByteSize(fileAttrData.nFileSizeLow, buffer, 50);
	sstream << buffer << L" | ";

	// file creation date
	SYSTEMTIME creationTime;
	FileTimeToSystemTime(&fileAttrData.ftCreationTime, &creationTime);
	sstream << creationTime.wYear << L"." << creationTime.wMonth << L"." << creationTime.wDay << L" | ";

	return sstream.str();
}

void FileProcessor::workerCheckSum(queue< wstring > * queueCheckSum)
{

	while(!queueCheckSum->empty()){

		csRead.lock();
			wstring filename = queueCheckSum->front();
			queueCheckSum->pop();
		csRead.unlock();
		
		//simple checksum
		DWORD checksum = 0;
		ifstream file(filename);
		string line;
		while (! file.eof() )
		{
			getline (file, line);

			for(string::iterator i = line.begin(); i != line.end(); i++)
			{
				checksum += *i;
			}
		}
		file.close();

		csWrite.lock();
			wstringstream sstream;
			sstream << checksum;
			wstring sum = sstream.str();
			fileCheckSum.insert( make_pair( filename, sum ) );
		csWrite.unlock();
	}
}