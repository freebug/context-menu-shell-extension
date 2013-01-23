#include "FileProcessor.h"


FileProcessor::FileProcessor(void)
{
	maxThreads = boost::thread::hardware_concurrency();
	if(!maxThreads)
		maxThreads = 2;
}

FileProcessor::~FileProcessor(void)
{
}

void FileProcessor::fileAdd(wstring filename)
{
	fileList.insert(filename);
}

const wstring FileProcessor::dumpFileInfo()
{

	//constructing message with information about every file
	if(!fileList.empty()){
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

		if(fileList.size() > 20)
			message.append(L"\n Out of total ")
				.append( boost::lexical_cast<std::wstring>(fileList.size()) )
				.append(L" files selected.");

		//clearing containers with file information
		fileDateSize.clear();
		fileCheckSum.clear();

		return message;
	}
	else
	{
		return L"";
	}
}

int FileProcessor::getFileInfo()
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

	return fileDateSize.size();
}

wstring FileProcessor::getFileSizeDate(WIN32_FILE_ATTRIBUTE_DATA fileAttrData)
{
	wstring result;

	//file size
	wchar_t buffer[50];
	StrFormatByteSize(fileAttrData.nFileSizeLow, buffer, 50);

	result = buffer;

	// file creation date
	SYSTEMTIME creationTime;
	FileTimeToSystemTime(&fileAttrData.ftCreationTime, &creationTime);

	result += L" | " + boost::lexical_cast<std::wstring>(creationTime.wYear) + L"." + boost::lexical_cast<std::wstring>(creationTime.wMonth) + L"." + boost::lexical_cast<std::wstring>(creationTime.wDay) + L" | ";
	
	return result;
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

		wstring sum = boost::lexical_cast<std::wstring>(checksum);
		csWrite.lock();
			fileCheckSum.insert( make_pair( filename, sum ) );
		csWrite.unlock();
	}
}

int FileProcessor::fileCount()
{
	return fileList.size();
}

int FileProcessor::filesClear()
{
	int size = fileList.size();
	if(size)
		fileList.clear();
	return size;
}