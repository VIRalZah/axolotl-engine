/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

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

Use any of these editors to generate BMFonts:
http://glyphdesigner.71squared.com/ (Commercial, Mac OS X)
http://www.n4te.com/hiero/hiero.jnlp (Free, Java)
http://slick.cokeandcode.com/demos/hiero.jnlp (Free, Java)
http://www.angelcode.com/products/bmfont/ (Free, Windows only)

****************************************************************************/
#include "LabelBMFont.h"
#include "base/String.h"
#include "platform/platform.h"
#include "base/Dictionary.h"
#include "base/Configuration.h"
#include "draw_nodes/DrawingPrimitives.h"
#include "sprite_nodes/Sprite.h"
#include "support/PointExtension.h"
#include "platform/FileUtils.h"
#include "base/Director.h"
#include "textures/TextureCache.h"
#include "support/ccUTF8.h"

using namespace std;

NS_AX_BEGIN

static unsigned short* copyUTF16StringN(unsigned short* str)
{
    int length = str ? AX_wcslen(str) : 0;
    unsigned short* ret = new unsigned short[length + 1];
    for (int i = 0; i < length; ++i) {
        ret[i] = str[i];
    }
    ret[length] = 0;
    return ret;
}

LabelBMFont * LabelBMFont::create()
{
    LabelBMFont * pRet = new LabelBMFont();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

LabelBMFont * LabelBMFont::create(const char *str, const char *fntFile, float width, CCTextAlignment alignment)
{
    return LabelBMFont::create(str, fntFile, width, alignment, Vec2::ZERO);
}

LabelBMFont * LabelBMFont::create(const char *str, const char *fntFile, float width)
{
    return LabelBMFont::create(str, fntFile, width, kCCTextAlignmentLeft, Vec2::ZERO);
}

LabelBMFont * LabelBMFont::create(const char *str, const char *fntFile)
{
    return LabelBMFont::create(str, fntFile, kCCLabelAutomaticWidth, kCCTextAlignmentLeft, Vec2::ZERO);
}

//LabelBMFont - Creation & Init
LabelBMFont *LabelBMFont::create(const char *str, const char *fntFile, float width/* = kCCLabelAutomaticWidth*/, CCTextAlignment alignment/* = kCCTextAlignmentLeft*/, Vec2 imageOffset/* = Vec2::ZERO*/)
{
    LabelBMFont *pRet = new LabelBMFont();
    if(pRet && pRet->initWithString(str, fntFile, width, alignment, imageOffset))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool LabelBMFont::init()
{
    return initWithString(NULL, NULL, kCCLabelAutomaticWidth, kCCTextAlignmentLeft, Vec2::ZERO);
}

bool LabelBMFont::initWithString(const char *theString, const char *fntFile, float width/* = kCCLabelAutomaticWidth*/, CCTextAlignment alignment/* = kCCTextAlignmentLeft*/, Vec2 imageOffset/* = Vec2::ZERO*/)
{
    AXAssert(!m_pConfiguration, "re-init is no longer supported");
    AXAssert( (theString && fntFile) || (theString==NULL && fntFile==NULL), "Invalid params for LabelBMFont");
    
    Texture2D *texture = NULL;
    
    if (fntFile)
    {
        BMFontConfiguration *newConf = BMFontCache::sharedBMFontCache()->addConfigurationWithFile(fntFile);
        if (!newConf)
        {
            AXLOG("cocos2d: WARNING. LabelBMFont: Impossible to create font. Please check file: '%s'", fntFile);
            release();
            return false;
        }
        
        newConf->retain();
        AX_SAFE_RELEASE(m_pConfiguration);
        m_pConfiguration = newConf;
        
        m_sFntFile = fntFile;
        
        texture = TextureCache::sharedTextureCache()->addImage(m_pConfiguration->_atlasName.c_str());
    }
    else 
    {
        texture = new Texture2D();
        texture->autorelease();
    }

    if (theString == NULL)
    {
        theString = "";
    }

    if (SpriteBatchNode::initWithTexture(texture, strlen(theString)))
    {
        m_fWidth = width;
        m_pAlignment = alignment;
        
        m_cDisplayedOpacity = m_cRealOpacity = 255;
        m_tDisplayedColor = m_tRealColor = ccWHITE;
        m_bCascadeOpacityEnabled = true;
        m_bCascadeColorEnabled = true;
        
        m_obContentSize = Size::ZERO;
        
        m_bIsOpacityModifyRGB = m_pobTextureAtlas->getTexture()->hasPremultipliedAlpha();
        m_obAnchorPoint = Vec2(0.5f, 0.5f);
        
        m_tImageOffset = imageOffset;
        
        m_pReusedChar = new Sprite();
        m_pReusedChar->initWithTexture(m_pobTextureAtlas->getTexture(), Rect(0, 0, 0, 0), false);
        m_pReusedChar->setBatchNode(this);
        
        this->setString(theString, true);
        
        return true;
    }
    return false;
}

LabelBMFont::LabelBMFont()
: m_sString(NULL)
, m_sInitialString(NULL)
, m_pAlignment(kCCTextAlignmentCenter)
, m_fWidth(-1.0f)
, m_pConfiguration(NULL)
, m_bLineBreakWithoutSpaces(false)
, m_tImageOffset(Vec2::ZERO)
, m_pReusedChar(NULL)
, m_cDisplayedOpacity(255)
, m_cRealOpacity(255)
, m_tDisplayedColor(ccWHITE)
, m_tRealColor(ccWHITE)
, m_bCascadeColorEnabled(true)
, m_bCascadeOpacityEnabled(true)
, m_bIsOpacityModifyRGB(false)
{

}

LabelBMFont::~LabelBMFont()
{
    AX_SAFE_RELEASE(m_pReusedChar);
    AX_SAFE_DELETE_ARRAY(m_sString);
    AX_SAFE_DELETE_ARRAY(m_sInitialString);
    AX_SAFE_RELEASE(m_pConfiguration);
}

// LabelBMFont - Atlas generation
int LabelBMFont::kerningAmountForFirst(unsigned short first, unsigned short second)
{
    int ret = 0;
    unsigned int key = (first<<16) | (second & 0xffff);

    if( m_pConfiguration->_kerningDictionary ) {
        axKerningHashElement *element = NULL;
        HASH_FIND_INT(m_pConfiguration->_kerningDictionary, &key, element);        
        if(element)
            ret = element->amount;
    }
    return ret;
}

void LabelBMFont::createFontChars()
{
    int nextFontPositionX = 0;
    int nextFontPositionY = 0;
    unsigned short prev = -1;
    int kerningAmount = 0;

    Size tmpSize = Size::ZERO;

    int longestLine = 0;
    unsigned int totalHeight = 0;

    unsigned int quantityOfLines = 1;
    unsigned int stringLen = m_sString ? AX_wcslen(m_sString) : 0;
    if (stringLen == 0)
    {
        this->setContentSize(AX_SIZE_PIXELS_TO_POINTS(tmpSize));
        return;
    }

    auto& charSet = m_pConfiguration->_characterSet;

    for (unsigned int i = 0; i < stringLen - 1; ++i)
    {
        unsigned short c = m_sString[i];
        if (c == '\n')
        {
            quantityOfLines++;
        }
    }

    totalHeight = m_pConfiguration->_commonHeight * quantityOfLines;
    nextFontPositionY = 0-(m_pConfiguration->_commonHeight - m_pConfiguration->_commonHeight * quantityOfLines);
    
    Rect rect;
    axBMFontDef fontDef;

    for (unsigned int i= 0; i < stringLen; i++)
    {
        unsigned short c = m_sString[i];

        if (c == '\n')
        {
            nextFontPositionX = 0;
            nextFontPositionY -= m_pConfiguration->_commonHeight;
            continue;
        }
        
        if (charSet.find(c) == charSet.end())
        {
            if (c != ' ')
            {
                AXLOGWARN("axolotl::LabelBMFont: Attempted to use character not defined in this bitmap: %d", c);
            }
            continue;      
        }

        kerningAmount = this->kerningAmountForFirst(prev, c);
        
        axFontDefHashElement *element = NULL;

        // unichar is a short, and an int is needed on HASH_FIND_INT
        unsigned int key = c;
        HASH_FIND_INT(m_pConfiguration->_fontDefDictionary, &key, element);
        if (! element)
        {
            if (c != ' ')
            {
                AXLOGWARN("axolotl::LabelBMFont: characer not found %d", c);
            }
            continue;
        }

        fontDef = element->fontDef;

        rect = fontDef.rect;
        rect = AX_RECT_PIXELS_TO_POINTS(rect);

        rect.origin.x += m_tImageOffset.x;
        rect.origin.y += m_tImageOffset.y;

        Sprite *fontChar;

        bool hasSprite = true;
        fontChar = (Sprite*)(this->getChildByTag(i));
        if(fontChar )
        {
            // Reusing previous Sprite
            fontChar->setVisible(true);
        }
        else
        {
            // New Sprite ? Set correct color, opacity, etc...
            if( 0 )
            {
                /* WIP: Doesn't support many features yet.
                 But this code is super fast. It doesn't create any sprite.
                 Ideal for big labels.
                 */
                fontChar = m_pReusedChar;
                fontChar->setBatchNode(NULL);
                hasSprite = false;
            }
            else
            {
                fontChar = new Sprite();
                fontChar->initWithTexture(m_pobTextureAtlas->getTexture(), rect);
                addChild(fontChar, i, i);
                fontChar->release();
            }
            
            // Apply label properties
            fontChar->setOpacityModifyRGB(m_bIsOpacityModifyRGB);
            
            // Color MUST be set before opacity, since opacity might change color if OpacityModifyRGB is on
            fontChar->updateDisplayedColor(m_tDisplayedColor);
            fontChar->updateDisplayedOpacity(m_cDisplayedOpacity);
        }

        // updating previous sprite
        fontChar->setTextureRect(rect, false, rect.size);

        // See issue 1343. cast( signed short + unsigned integer ) == unsigned integer (sign is lost!)
        int yOffset = m_pConfiguration->_commonHeight - fontDef.yOffset;
        Vec2 fontPos = Vec2( (float)nextFontPositionX + fontDef.xOffset + fontDef.rect.size.width*0.5f + kerningAmount,
            (float)nextFontPositionY + yOffset - rect.size.height*0.5f * AX_CONTENT_SCALE_FACTOR() );
        fontChar->setPosition(AX_POINT_PIXELS_TO_POINTS(fontPos));

        // update kerning
        nextFontPositionX += fontDef.xAdvance + kerningAmount;
        prev = c;

        if (longestLine < nextFontPositionX)
        {
            longestLine = nextFontPositionX;
        }
        
        if (! hasSprite)
        {
            updateQuadFromSprite(fontChar, i);
        }
    }

    // If the last character processed has an xAdvance which is less that the width of the characters image, then we need
    // to adjust the width of the string to take this into account, or the character will overlap the end of the bounding
    // box
    if (fontDef.xAdvance < fontDef.rect.size.width)
    {
        tmpSize.width = longestLine + fontDef.rect.size.width - fontDef.xAdvance;
    }
    else
    {
        tmpSize.width = longestLine;
    }
    tmpSize.height = totalHeight;

    this->setContentSize(AX_SIZE_PIXELS_TO_POINTS(tmpSize));
}

//LabelBMFont - LabelProtocol protocol
void LabelBMFont::setString(const std::string& newString)
{
    this->setString(newString, true);
}

void LabelBMFont::setString(const std::string& newString, bool needUpdateLabel)
{
    if (needUpdateLabel) {
        m_sInitialStringUTF8 = newString;
    }
    unsigned short* utf16String = AX_utf8_to_utf16(newString.c_str());
    setString(utf16String, needUpdateLabel);
    AX_SAFE_DELETE_ARRAY(utf16String);
 }

void LabelBMFont::setString(unsigned short *newString, bool needUpdateLabel)
{
    if (!needUpdateLabel)
    {
        unsigned short* tmp = m_sString;
        m_sString = copyUTF16StringN(newString);
        AX_SAFE_DELETE_ARRAY(tmp);
    }
    else
    {
        unsigned short* tmp = m_sInitialString;
        m_sInitialString = copyUTF16StringN(newString);
        AX_SAFE_DELETE_ARRAY(tmp);
    }
    
    if (m_pChildren && m_pChildren->count() != 0)
    {
        Object* child;
        AXARRAY_FOREACH(m_pChildren, child)
        {
            Node* pNode = (Node*) child;
            if (pNode)
            {
                pNode->setVisible(false);
            }
        }
    }
    this->createFontChars();
    
    if (needUpdateLabel) {
        updateLabel();
    }
}

const std::string& LabelBMFont::getString(void)
{
    return m_sInitialStringUTF8;
}

void LabelBMFont::setCString(const char *label)
{
    setString(label);
}

//LabelBMFont - RGBAProtocol protocol
const ccColor3B& LabelBMFont::getColor()
{
    return m_tRealColor;
}

const ccColor3B& LabelBMFont::getDisplayedColor()
{
    return m_tDisplayedColor;
}

void LabelBMFont::setColor(const ccColor3B& color)
{
    m_tDisplayedColor = m_tRealColor = color;
    
    if( m_bCascadeColorEnabled ) {
        ccColor3B parentColor = ccWHITE;
        RGBAProtocol* pParent = dynamic_cast<RGBAProtocol*>(m_pParent);
        if (pParent && pParent->isCascadeColorEnabled())
        {
            parentColor = pParent->getDisplayedColor();
        }
        this->updateDisplayedColor(parentColor);
    }
}

GLubyte LabelBMFont::getOpacity(void)
{
    return m_cRealOpacity;
}

GLubyte LabelBMFont::getDisplayedOpacity(void)
{
    return m_cDisplayedOpacity;
}

/** Override synthesized setOpacity to recurse items */
void LabelBMFont::setOpacity(GLubyte opacity)
{
    m_cDisplayedOpacity = m_cRealOpacity = opacity;
    
    if( m_bCascadeOpacityEnabled ) {
        GLubyte parentOpacity = 255;
        RGBAProtocol* pParent = dynamic_cast<RGBAProtocol*>(m_pParent);
        if (pParent && pParent->isCascadeOpacityEnabled())
        {
            parentOpacity = pParent->getDisplayedOpacity();
        }
        this->updateDisplayedOpacity(parentOpacity);
    }
}

void LabelBMFont::setOpacityModifyRGB(bool var)
{
    m_bIsOpacityModifyRGB = var;
    if (m_pChildren && m_pChildren->count() != 0)
    {
        Object* child;
        AXARRAY_FOREACH(m_pChildren, child)
        {
            Node* pNode = (Node*) child;
            if (pNode)
            {
                RGBAProtocol *pRGBAProtocol = dynamic_cast<RGBAProtocol*>(pNode);
                if (pRGBAProtocol)
                {
                    pRGBAProtocol->setOpacityModifyRGB(m_bIsOpacityModifyRGB);
                }
            }
        }
    }
}
bool LabelBMFont::isOpacityModifyRGB()
{
    return m_bIsOpacityModifyRGB;
}

void LabelBMFont::updateDisplayedOpacity(GLubyte parentOpacity)
{
    m_cDisplayedOpacity = m_cRealOpacity * parentOpacity/255.0;
    
    Object* pObj;
    AXARRAY_FOREACH(m_pChildren, pObj)
    {
        Sprite *item = (Sprite*)pObj;
        item->updateDisplayedOpacity(m_cDisplayedOpacity);
    }
}

void LabelBMFont::updateDisplayedColor(const ccColor3B& parentColor)
{
    m_tDisplayedColor.r = m_tRealColor.r * parentColor.r/255.0;
    m_tDisplayedColor.g = m_tRealColor.g * parentColor.g/255.0;
    m_tDisplayedColor.b = m_tRealColor.b * parentColor.b/255.0;
    
    Object* pObj;
    AXARRAY_FOREACH(m_pChildren, pObj)
    {
        Sprite *item = (Sprite*)pObj;
        item->updateDisplayedColor(m_tDisplayedColor);
    }
}

bool LabelBMFont::isCascadeColorEnabled()
{
    return false;
}

void LabelBMFont::setCascadeColorEnabled(bool cascadeColorEnabled)
{
    m_bCascadeColorEnabled = cascadeColorEnabled;
}

bool LabelBMFont::isCascadeOpacityEnabled()
{
    return false;
}

void LabelBMFont::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
    m_bCascadeOpacityEnabled = cascadeOpacityEnabled;
}

// LabelBMFont - AnchorPoint
void LabelBMFont::setAnchorPoint(const Vec2& point)
{
    if( ! point.equals(m_obAnchorPoint))
    {
        SpriteBatchNode::setAnchorPoint(point);
        updateLabel();
    }
}

// LabelBMFont - Alignment
void LabelBMFont::updateLabel()
{
    this->setString(m_sInitialString, false);

    if (m_fWidth > 0)
    {
        // Step 1: Make multiline
        vector<unsigned short> str_whole = AX_utf16_vec_from_utf16_str(m_sString);
        unsigned int stringLength = str_whole.size();
        vector<unsigned short> multiline_string;
        multiline_string.reserve( stringLength );
        vector<unsigned short> last_word;
        last_word.reserve( stringLength );

        unsigned int line = 1, i = 0;
        bool start_line = false, start_word = false;
        float startOfLine = -1, startOfWord = -1;
        int skip = 0;

        Array* children = getChildren();
        for (unsigned int j = 0; j < children->count(); j++)
        {
            Sprite* characterSprite;
            unsigned int justSkipped = 0;
            
            while (!(characterSprite = (Sprite*)this->getChildByTag(j + skip + justSkipped)))
            {
                justSkipped++;
            }
            
            skip += justSkipped;
            
            if (!characterSprite->isVisible())
                continue;

            if (i >= stringLength)
                break;

            unsigned short character = str_whole[i];

            if (!start_word)
            {
                startOfWord = getLetterPosXLeft( characterSprite );
                start_word = true;
            }
            if (!start_line)
            {
                startOfLine = startOfWord;
                start_line = true;
            }

            // Newline.
            if (character == '\n')
            {
                AX_utf8_trim_ws(&last_word);

                last_word.push_back('\n');
                multiline_string.insert(multiline_string.end(), last_word.begin(), last_word.end());
                last_word.clear();
                start_word = false;
                start_line = false;
                startOfWord = -1;
                startOfLine = -1;
                i+=justSkipped;
                line++;

                if (i >= stringLength)
                    break;

                character = str_whole[i];

                if (!startOfWord)
                {
                    startOfWord = getLetterPosXLeft( characterSprite );
                    start_word = true;
                }
                if (!startOfLine)
                {
                    startOfLine  = startOfWord;
                    start_line = true;
                }
            }

            // Whitespace.
            if (isspace_unicode(character))
            {
                last_word.push_back(character);
                multiline_string.insert(multiline_string.end(), last_word.begin(), last_word.end());
                last_word.clear();
                start_word = false;
                startOfWord = -1;
                i++;
                continue;
            }

            // Out of bounds.
            if ( getLetterPosXRight( characterSprite ) - startOfLine > m_fWidth )
            {
                if (!m_bLineBreakWithoutSpaces)
                {
                    last_word.push_back(character);

                    int found = AX_utf8_find_last_not_char(multiline_string, ' ');
                    if (found != -1)
                        AX_utf8_trim_ws(&multiline_string);
                    else
                        multiline_string.clear();

                    if (multiline_string.size() > 0)
                        multiline_string.push_back('\n');

                    line++;
                    start_line = false;
                    startOfLine = -1;
                    i++;
                }
                else
                {
                    AX_utf8_trim_ws(&last_word);

                    last_word.push_back('\n');
                    multiline_string.insert(multiline_string.end(), last_word.begin(), last_word.end());
                    last_word.clear();
                    start_word = false;
                    start_line = false;
                    startOfWord = -1;
                    startOfLine = -1;
                    line++;

                    if (i >= stringLength)
                        break;

                    if (!startOfWord)
                    {
                        startOfWord = getLetterPosXLeft( characterSprite );
                        start_word = true;
                    }
                    if (!startOfLine)
                    {
                        startOfLine  = startOfWord;
                        start_line = true;
                    }

                    j--;
                }

                continue;
            }
            else
            {
                // Character is normal.
                last_word.push_back(character);
                i++;
                continue;
            }
        }

        multiline_string.insert(multiline_string.end(), last_word.begin(), last_word.end());

        int size = multiline_string.size();
        unsigned short* str_new = new unsigned short[size + 1];

        for (int i = 0; i < size; ++i)
        {
            str_new[i] = multiline_string[i];
        }

        str_new[size] = '\0';

        this->setString(str_new, false);
        
        AX_SAFE_DELETE_ARRAY(str_new);
    }

    // Step 2: Make alignment
    if (m_pAlignment != kCCTextAlignmentLeft)
    {
        int i = 0;

        int lineNumber = 0;
        int str_len = AX_wcslen(m_sString);
        vector<unsigned short> last_line;
        for (int ctr = 0; ctr <= str_len; ++ctr)
        {
            if (m_sString[ctr] == '\n' || m_sString[ctr] == 0)
            {
                float lineWidth = 0.0f;
                unsigned int line_length = last_line.size();
                // if last line is empty we must just increase lineNumber and work with next line
                if (line_length == 0)
                {
                    lineNumber++;
                    continue;
                }
                int index = i + line_length - 1 + lineNumber;
                if (index < 0) continue;

                Sprite* lastChar = (Sprite*)getChildByTag(index);
                if ( lastChar == NULL )
                    continue;

                lineWidth = lastChar->getPosition().x + lastChar->getContentSize().width/2.0f;

                float shift = 0;
                switch (m_pAlignment)
                {
                case kCCTextAlignmentCenter:
                    shift = getContentSize().width/2.0f - lineWidth/2.0f;
                    break;
                case kCCTextAlignmentRight:
                    shift = getContentSize().width - lineWidth;
                    break;
                default:
                    break;
                }

                if (shift != 0)
                {
                    for (unsigned j = 0; j < line_length; j++)
                    {
                        index = i + j + lineNumber;
                        if (index < 0) continue;

                        Sprite* characterSprite = (Sprite*)getChildByTag(index);
                        if (characterSprite)
                        {
                            characterSprite->setPosition(PointAdd(characterSprite->getPosition(), Vec2(shift, 0.0f)));
                        }
                    }
                }

                i += line_length;
                lineNumber++;

                last_line.clear();
                continue;
            }

            last_line.push_back(m_sString[ctr]);
        }
    }
}

// LabelBMFont - Alignment
void LabelBMFont::setAlignment(CCTextAlignment alignment)
{
    this->m_pAlignment = alignment;
    updateLabel();
}

void LabelBMFont::setWidth(float width)
{
    this->m_fWidth = width;
    updateLabel();
}

void LabelBMFont::setLineBreakWithoutSpace( bool breakWithoutSpace )
{
    m_bLineBreakWithoutSpaces = breakWithoutSpace;
    updateLabel();
}

void LabelBMFont::setScale(float scale)
{
    SpriteBatchNode::setScale(scale);
    updateLabel();
}

void LabelBMFont::setScaleX(float scaleX)
{
    SpriteBatchNode::setScaleX(scaleX);
    updateLabel();
}

void LabelBMFont::setScaleY(float scaleY)
{
    SpriteBatchNode::setScaleY(scaleY);
    updateLabel();
}

void LabelBMFont::limitLabelWidth(float maxContentSize, float minScale)
{
    auto width = getScaledContentSize().width;
    if (width > maxContentSize)
    {
        float scale = maxContentSize / width;
        if (scale < minScale)
        {
            scale = minScale;
        }
        setScale(scale);
    }
}

float LabelBMFont::getLetterPosXLeft( Sprite* sp )
{
    return sp->getPosition().x * _scaleX - (sp->getContentSize().width * _scaleX * sp->getAnchorPoint().x);
}

float LabelBMFont::getLetterPosXRight( Sprite* sp )
{
    return sp->getPosition().x * _scaleX + (sp->getContentSize().width * _scaleX * sp->getAnchorPoint().x);
}

// LabelBMFont - FntFile
void LabelBMFont::setFntFile(const char* fntFile)
{
    if (fntFile != NULL && strcmp(fntFile, m_sFntFile.c_str()) != 0 )
    {
        BMFontConfiguration *newConf = BMFontCache::sharedBMFontCache()->addConfigurationWithFile(fntFile);

        AXAssert( newConf, "LabelBMFont: Impossible to create font. Please check file");

        m_sFntFile = fntFile;

        AX_SAFE_RETAIN(newConf);
        AX_SAFE_RELEASE(m_pConfiguration);
        m_pConfiguration = newConf;

        this->setTexture(TextureCache::sharedTextureCache()->addImage(m_pConfiguration->_atlasName.c_str()));
        this->createFontChars();
    }
}

const char* LabelBMFont::getFntFile()
{
    return m_sFntFile.c_str();
}

BMFontConfiguration* LabelBMFont::getConfiguration() const
{
    return m_pConfiguration;
}

//LabelBMFont - Debug draw
#if AX_LABELBMFONT_DEBUG_DRAW
void LabelBMFont::draw()
{
    SpriteBatchNode::draw();
    const Size& s = this->getContentSize();
    Vec2 vertices[4]={
        Vec2(0,0),Vec2(s.width,0),
        Vec2(s.width,s.height),Vec2(0,s.height),
    };
    ccDrawPoly(vertices, 4, true);
}

#endif // AX_LABELBMFONT_DEBUG_DRAW

NS_AX_END
