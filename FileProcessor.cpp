#include "FileProcessor.h"


FileProcessor::FileProcessor(void)
{
}


FileProcessor::~FileProcessor(void)
{
}

void FileProcessor::addFile(wstring filename)
{
	fileList.insert(filename);
}

const wstring FileProcessor::dumpFileInfo()
{
	wstring message;

	//constructing message
	message.append(L"File list:\n\n");
	for each (std::wstring filename in fileList)
	{
		size_t delim = filename.find_last_of(L"\\");
		if( std::wstring::npos != delim )
			filename = filename.substr(delim+1);
		message.append(filename).append(L"\n");
	}

	return message;
}