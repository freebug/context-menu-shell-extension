#include "FileProcessor.h"


FileProcessor::FileProcessor(void)
{
	InitializeCriticalSection(&cs);
}


FileProcessor::~FileProcessor(void)
{
	DeleteCriticalSection(&cs);
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


		//create a new thread
		pair<wstring, FileProcessor*> *args = new pair<wstring, FileProcessor*>;
		args->first = filename;
		args->second = this;
		handleList.push_back( (HANDLE)_beginthreadex(0, 0, getCheckSum, args, 0, 0) );

	}
	//waiting for threads to end
	WaitForMultipleObjects(handleList.size(), &handleList[0], 1, INFINITE);

}

wstring FileProcessor::getFileSizeDate(WIN32_FILE_ATTRIBUTE_DATA fileAttrData)
{
	wstringstream sstream;

	// file size
	// wchar_t buffer[50];
	//StrFormatByteSize(fileAttrData.nFileSizeLow, buffer, 50);
	//sstream << buffer << L" | ";

	// file creation date
	SYSTEMTIME creationTime;
	FileTimeToSystemTime(&fileAttrData.ftCreationTime, &creationTime);
	sstream << creationTime.wYear << L"." << creationTime.wMonth << L"." << creationTime.wDay << L" | ";

	return sstream.str();
}

unsigned __stdcall FileProcessor::getCheckSum(void* args)
{
	wstring filename;
	pair<wstring, FileProcessor*> *params;
	params = static_cast< pair<wstring, FileProcessor*>* >(args);

	//simple checksum
	DWORD checksum = 0;
	ifstream file(params->first);
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
	EnterCriticalSection(&cs);
	params->second->saveCheckSum(params->first, checksum);
	LeaveCriticalSection(&cs);
	return 0;
}

void FileProcessor::saveCheckSum(wstring filename, DWORD checksum)
{
	wstringstream sstream;
	sstream << checksum;
	wstring sum = sstream.str();
	fileCheckSum.insert( make_pair( filename, sum ) );
}