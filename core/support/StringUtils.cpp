#include "StringUtils.h"

NS_AX_BEGIN

std::string StringUtils::format(const char* format, ...)
{
    char buffer[MAX_LEN] = { 0 };

    va_list ap;
    va_start(ap, format);
    int len = vsnprintf_s(buffer, MAX_LEN, MAX_LEN, format, ap);
    va_end(ap);

    return std::string(buffer, len);
}

std::string StringUtils::codePointToUTF8(unsigned int codepoint)
{
    std::string ret("");
    
    if (codepoint <= 0x7F)
    {
        ret.push_back(codepoint);
    }
    else if (codepoint <= 0x7FF)
    {
        ret.push_back(0xC0 | ((codepoint >> 6) & 0x1F));
        ret.push_back(0x80 | (codepoint & 0x3F));
    }
    else if (codepoint <= 0xFFFF)
    {
        ret.push_back(0xE0 | ((codepoint >> 12) & 0x0F));
        ret.push_back(0x80 | ((codepoint >> 6) & 0x3F));
        ret.push_back(0x80 | (codepoint & 0x3F));
    }
    else if (codepoint <= 0x10FFFF)
    {
        ret.push_back(0xF0 | ((codepoint >> 18) & 0x07));
        ret.push_back(0x80 | ((codepoint >> 12) & 0x3F));
        ret.push_back(0x80 | ((codepoint >> 6) & 0x3F));
        ret.push_back(0x80 | (codepoint & 0x3F));
    }
    else
    {
        AXAssert(false, "invalid codepoint!");
    }

    return ret;
}

NS_AX_END