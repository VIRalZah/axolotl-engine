/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org  http://cocos2d-x.org
 Copyright (c) 2010 Максим Аксенов
 Copyright (c) 2013 Martell Malone
 
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

#include "SAXParser.h"
#include "base/Dictionary.h"
#include "FileUtils.h"
#include "support/tinyxml2/tinyxml2.h"

#include <vector> // because its based on windows 8 build :P

NS_AX_BEGIN

class XmlSaxHander : public tinyxml2::XMLVisitor
{
public:
	XmlSaxHander():m_ccsaxParserImp(0){};
	
	virtual bool VisitEnter( const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* firstAttribute );
	virtual bool VisitExit( const tinyxml2::XMLElement& element );
	virtual bool Visit( const tinyxml2::XMLText& text );
	virtual bool Visit( const tinyxml2::XMLUnknown&){ return true; }

	void setCCSAXParserImp(SAXParser* parser)
	{
		m_ccsaxParserImp = parser;
	}

private:
	SAXParser *m_ccsaxParserImp;
};


bool XmlSaxHander::VisitEnter( const tinyxml2::XMLElement& element, const tinyxml2::XMLAttribute* firstAttribute )
{
	//log(" VisitEnter %s",element.Value());

	std::vector<const char*> attsVector;
	for( const tinyxml2::XMLAttribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
	{
		//log("%s", attrib->Name());
		attsVector.push_back(attrib->Name());
		//log("%s",attrib->Value());
		attsVector.push_back(attrib->Value());
	}
    
    // nullptr is used in c++11
	//attsVector.push_back(nullptr);
    attsVector.push_back(NULL);

	SAXParser::startElement(m_ccsaxParserImp, (const AX_XML_CHAR *)element.Value(), (const AX_XML_CHAR **)(&attsVector[0]));
	return true;
}
bool XmlSaxHander::VisitExit( const tinyxml2::XMLElement& element )
{
	//log("VisitExit %s",element.Value());

	SAXParser::endElement(m_ccsaxParserImp, (const AX_XML_CHAR *)element.Value());
	return true;
}

bool XmlSaxHander::Visit( const tinyxml2::XMLText& text )
{
	//log("Visit %s",text.Value());
	SAXParser::textHandler(m_ccsaxParserImp, (const AX_XML_CHAR *)text.Value(), strlen(text.Value()));
	return true;
}

SAXParser::SAXParser()
{
    m_pDelegator = NULL;
}

SAXParser::~SAXParser(void)
{
}

bool SAXParser::init(const char *pszEncoding)
{
    AX_UNUSED_PARAM(pszEncoding);
    // nothing to do
    return true;
}

bool SAXParser::parse(const char* pXMLData, unsigned int uDataLength)
{
	tinyxml2::XMLDocument tinyDoc;
	tinyDoc.Parse(pXMLData, uDataLength);
	XmlSaxHander printer;
	printer.setCCSAXParserImp(this);
	
	return tinyDoc.Accept( &printer );	
}

bool SAXParser::parse(const char *pszFile)
{
    bool bRet = false;
    unsigned long size = 0;
    char* pBuffer = (char*)FileUtils::sharedFileUtils()->getFileData(pszFile, "rt", &size);
    if (pBuffer != NULL && size > 0)
    {
        bRet = parse(pBuffer, size);
    }
    AX_SAFE_DELETE_ARRAY(pBuffer);
    return bRet;
}

void SAXParser::startElement(void *ctx, const AX_XML_CHAR *name, const AX_XML_CHAR **atts)
{
    ((SAXParser*)(ctx))->m_pDelegator->startElement(ctx, (char*)name, (const char**)atts);
}

void SAXParser::endElement(void *ctx, const AX_XML_CHAR *name)
{
    ((SAXParser*)(ctx))->m_pDelegator->endElement(ctx, (char*)name);
}
void SAXParser::textHandler(void *ctx, const AX_XML_CHAR *name, int len)
{
    ((SAXParser*)(ctx))->m_pDelegator->textHandler(ctx, (char*)name, len);
}
void SAXParser::setDelegator(SAXDelegator* pDelegator)
{
    m_pDelegator = pDelegator;
}

NS_AX_END


