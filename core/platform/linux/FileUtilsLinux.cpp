/*
 * FileUtilsLinux.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: laschweinski
 */
#include "FileUtilsLinux.h"
#include "platform/Common.h"
#include "ccMacros.h"
#include "Application.h"
#include "cocoa/String.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

NS_AX_BEGIN

FileUtils* FileUtils::sharedFileUtils()
{
    if (s_sharedFileUtils == NULL)
    {
        s_sharedFileUtils = new FileUtilsLinux();
        s_sharedFileUtils->init();
    }
    return s_sharedFileUtils;
}

FileUtilsLinux::FileUtilsLinux()
{}

bool FileUtilsLinux::init()
{
    // get application path
    char fullpath[256] = {0};
    ssize_t length = readlink("/proc/self/exe", fullpath, sizeof(fullpath)-1);

    if (length <= 0) {
        return false;
    }

    fullpath[length] = '\0';
    std::string appPath = fullpath;
    m_strDefaultResRootPath = appPath.substr(0, appPath.find_last_of("/"));
    m_strDefaultResRootPath += "/../../../Resources/";

    // Set writable path to $XDG_CONFIG_HOME or ~/.config/<app name>/ if $XDG_CONFIG_HOME not exists.
    const char* xdg_config_path = getenv("XDG_CONFIG_HOME");
    std::string xdgConfigPath;
    if (xdg_config_path == NULL) {
        xdgConfigPath = getenv("HOME");
        xdgConfigPath += "/.config";
    } else {
        xdgConfigPath  = xdg_config_path;
    }
    m_writablePath = xdgConfigPath;
    m_writablePath += appPath.substr(appPath.find_last_of("/"));
    m_writablePath += "/";

    return FileUtils::init();
}

string FileUtilsLinux::getWritablePath()
{
    struct stat st;
    stat(m_writablePath.c_str(), &st);
    if (!S_ISDIR(st.st_mode)) {
        mkdir(m_writablePath.c_str(), 0744);
    }

    return m_writablePath;
}

bool FileUtilsLinux::isFileExist(const std::string& strFilePath)
{
    if (0 == strFilePath.length())
    {
        return false;
    }

    std::string strPath = strFilePath;
    if (!isAbsolutePath(strPath))
    { // Not absolute path, add the default root path at the beginning.
        strPath.insert(0, m_strDefaultResRootPath);
    }
    
    struct stat sts;
    return (stat(strPath.c_str(), &sts) != -1) ? true : false;
}

NS_AX_END
