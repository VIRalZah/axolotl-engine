/****************************************************************************
Copyright (c) 2024 zahann.ru

http://www.zahann.ru

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

#include "StringUtils.h"
#include <codecvt>

NS_AX_BEGIN

namespace StringUtils
{
    std::string format(const char* format, ...)
    {
        char buffer[MAX_LEN] = { 0 };

        va_list ap;
        va_start(ap, format);
        int len = vsnprintf_s(buffer, MAX_LEN, MAX_LEN, format, ap);
        va_end(ap);

        return std::string(buffer, len);
    }

    std::string boolToString(bool v)
    {
        return v ? "true" : "false";
    }

    bool stringToBool(const std::string& str)
    {
        if (str == "true" || str == "1")
        {
            return true;
        }
        return false;
    }
}

NS_AX_END