#include <cstdlib>
#include <iostream>
#include <boost\lexical_cast.hpp>
#include "gtest\gtest.h"
#include "..\FileProcessor.h"
#include "..\FileProcessor.cpp"

class FileProcessorTest : public testing::Test {
 protected:  
	
	virtual void SetUp() {
		TestProcessor = new FileProcessor;
	}

	virtual void TearDown() {
		delete TestProcessor;
	}

	void Fill()
	{

	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	
	wstring testdir = _wgetenv(L"COMSPEC"); //defined _CRT_SECURE_NO_WARNINGS to stop this from producing error on MS VS 2012
	testdir.erase(testdir.find_last_of(L"\\")).append(L"\\*");

	hFind = FindFirstFile(testdir.c_str(), &ffd);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
				wstring filename = testdir;
				filename.replace(filename.end()-1, filename.end(), ffd.cFileName);
				TestProcessor->fileAdd(filename);
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
	}
	}

	void Clear()
	{
		TestProcessor->filesClear();
	}

	FileProcessor* TestProcessor;
};

//creating object
TEST_F(FileProcessorTest, Constructor) {
	EXPECT_EQ(0, TestProcessor->fileCount());
}

//filling object with files and clearing it
TEST_F(FileProcessorTest, FileListContainer) {
	Fill();
	EXPECT_GT(TestProcessor->fileCount(), 0);
	Clear();
	EXPECT_EQ(0, TestProcessor->fileCount());
}

//testing return values of filesClear method 
TEST_F(FileProcessorTest, filesClear) {
	Fill();
	int count = TestProcessor->fileCount();
	EXPECT_EQ(count, TestProcessor->filesClear());
	EXPECT_EQ(0, TestProcessor->filesClear());
}

//testing return values of dumpFileInfo method
TEST_F(FileProcessorTest, dumpFileInfo) {
	Fill();
	EXPECT_EQ(L'F', TestProcessor->dumpFileInfo()[0]);
	EXPECT_GT(TestProcessor->fileCount(), 0); 
	Clear();
	EXPECT_EQ(L"", TestProcessor->dumpFileInfo());
}