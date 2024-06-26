/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "NS.h"
#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>

using namespace std;

NS_AX_BEGIN

void splitString(const std::string& string, const char* token, strArray& array)
{
    int end = 0;
    int begin = 0;
    while (end != std::string::npos)
    {
        end = string.find(token, begin);
        if (end == std::string::npos)
        {
            array.push_back(string.substr(begin, string.length() - begin));
        }
        else
        {
            array.push_back(string.substr(begin, end - begin));
        }
        begin = end + strlen(token);
    }
}

// first, judge whether the form of the string like this: {x,y}
// if the form is right,the string will be split into the parameter strs;
// or the parameter strs will be empty.
// if the form is right return true,else return false.
static bool splitWithForm(const char* pStr, strArray& strs)
{
    bool bRet = false;

    do 
    {
        AX_BREAK_IF(!pStr);

        // string is empty
        std::string content = pStr;
        AX_BREAK_IF(content.length() == 0);

        int nPosLeft  = content.find('{');
        int nPosRight = content.find('}');

        // don't have '{' and '}'
        AX_BREAK_IF(nPosLeft == (int)std::string::npos || nPosRight == (int)std::string::npos);
        // '}' is before '{'
        AX_BREAK_IF(nPosLeft > nPosRight);

        std::string pointStr = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
        // nothing between '{' and '}'
        AX_BREAK_IF(pointStr.length() == 0);

        int nPos1 = pointStr.find('{');
        int nPos2 = pointStr.find('}');
        // contain '{' or '}' 
        AX_BREAK_IF(nPos1 != (int)std::string::npos || nPos2 != (int)std::string::npos);

        splitString(pointStr, ",", strs);
        if (strs.size() != 2 || strs[0].length() == 0 || strs[1].length() == 0)
        {
            strs.clear();
            break;
        }

        bRet = true;
    } while (0);

    return bRet;
}

// implement the functions

Rect CCRectFromString(const char* pszContent)
{
    Rect result = Rect::ZERO;

    do 
    {
        AX_BREAK_IF(!pszContent);
        std::string content = pszContent;

        // find the first '{' and the third '}'
        int nPosLeft  = content.find('{');
        int nPosRight = content.find('}');
        for (int i = 1; i < 3; ++i)
        {
            if (nPosRight == (int)std::string::npos)
            {
                break;
            }
            nPosRight = content.find('}', nPosRight + 1);
        }
        AX_BREAK_IF(nPosLeft == (int)std::string::npos || nPosRight == (int)std::string::npos);

        content = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
        int nPointEnd = content.find('}');
        AX_BREAK_IF(nPointEnd == (int)std::string::npos);
        nPointEnd = content.find(',', nPointEnd);
        AX_BREAK_IF(nPointEnd == (int)std::string::npos);

        // get the point string and size string
        std::string pointStr = content.substr(0, nPointEnd);
        std::string sizeStr  = content.substr(nPointEnd + 1, content.length() - nPointEnd);

        // split the string with ','
        strArray pointInfo;
        AX_BREAK_IF(!splitWithForm(pointStr.c_str(), pointInfo));
        strArray sizeInfo;
        AX_BREAK_IF(!splitWithForm(sizeStr.c_str(), sizeInfo));

        float x = (float) atof(pointInfo[0].c_str());
        float y = (float) atof(pointInfo[1].c_str());
        float width  = (float) atof(sizeInfo[0].c_str());
        float height = (float) atof(sizeInfo[1].c_str());

        result = Rect(x, y, width, height);
    } while (0);

    return result;
}

Vec2 CCPointFromString(const char* pszContent)
{
    Vec2 ret = Vec2::ZERO;

    do 
    {
        strArray strs;
        AX_BREAK_IF(!splitWithForm(pszContent, strs));

        float x = (float) atof(strs[0].c_str());
        float y = (float) atof(strs[1].c_str());

        ret = Vec2(x, y);
    } while (0);

    return ret;
}

Size CCSizeFromString(const char* pszContent)
{
    Size ret = Size::ZERO;

    do 
    {
        strArray strs;
        AX_BREAK_IF(!splitWithForm(pszContent, strs));

        float width  = (float) atof(strs[0].c_str());
        float height = (float) atof(strs[1].c_str());

        ret = Size(width, height);
    } while (0);

    return ret;
}

NS_AX_END
