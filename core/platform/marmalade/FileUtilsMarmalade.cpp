#include "FileUtilsMarmalade.h"
#include "platform/Common.h"
#include "ccMacros.h"
#include "Application.h"
#include "cocoa/String.h"
#include "IwDebug.h" // for Log

using namespace std;

NS_AX_BEGIN

FileUtils* FileUtils::sharedFileUtils()
{
    if (s_sharedFileUtils == NULL)
    {
        s_sharedFileUtils = new FileUtilsMarmalade();
        s_sharedFileUtils->init();
    }
    return s_sharedFileUtils;
}

FileUtilsMarmalade::FileUtilsMarmalade()
{}


string FileUtilsMarmalade::getWritablePath()
{
    return std::string("ram://");
}

bool FileUtilsMarmalade::isFileExist(const std::string& strFilePath)
{
    if (0 == strFilePath.length())
    {
        return false;
    }
    
    return s3eFileCheckExists(strFilePath.c_str()) == S3E_TRUE ? true : false;
}

unsigned char* FileUtilsMarmalade::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
	IW_CALLSTACK("FileUtils::getFileData");
    
    std::string fullPath = fullPathForFilename(pszFileName);
    
	s3eFile* pFile = s3eFileOpen(fullPath.c_str(), pszMode);
	
	if (! pFile && isPopupNotify())
	{
		IwAssertMsg(GAME, pFile, ("Open file %s Failed. s3eFileError Code : %i", pszFileName, s3eFileGetError()));
	}
	if (! pFile)
	{
		*pSize = 0;
		return 0;
	}
	int32 fileSize = s3eFileGetSize(pFile);
	*pSize=fileSize;
    
	static int32* pDataToBeReadBinary;
    
	pDataToBeReadBinary = (int32*)s3eMallocBase(fileSize);
	memset(pDataToBeReadBinary, 0, fileSize);
	s3eFileRead(pDataToBeReadBinary, fileSize, 1, pFile);
	s3eFileClose(pFile);
	
	return (unsigned char*)pDataToBeReadBinary;
}

bool FileUtilsMarmalade::isAbsolutePath(const std::string& strPath)
{
    if (strPath[0] == '/' || strPath.find("ram://") == 0)
    {
        return true;
    }
    return false;
}

NS_AX_END