/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada

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
#ifndef __AXLABELTTF_H__
#define __AXLABELTTF_H__

#include "sprite_nodes/Sprite.h"
#include "textures/Texture2D.h"

NS_AX_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup label
 * @{
 */



/** @brief LabelTTF is a subclass of CCTextureNode that knows how to render text labels
 *
 * All features from CCTextureNode are valid in LabelTTF
 *
 * LabelTTF objects are slow. Consider using LabelAtlas or LabelBMFont instead.
 *
 * Custom ttf file can be put in assets/ or external storage that the Application can access.
 * @code
 * LabelTTF *label1 = LabelTTF::create("alignment left", "A Damn Mess", fontSize, blockSize, 
 *                                          kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
 * LabelTTF *label2 = LabelTTF::create("alignment right", "/mnt/sdcard/Scissor Cuts.ttf", fontSize, blockSize,
 *                                          kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
 * @endcode
 *
 */
class AX_DLL LabelTTF : public Sprite, public LabelProtocol
{
public:
    /**
     *  @js ctor
     */
    LabelTTF();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~LabelTTF();
    /**
     *  @js NA
     *  @lua NA
     */
    const char* description();
    
    /** creates a LabelTTF with a font name and font size in points
     @since v2.0.1
     */
    static LabelTTF * create(const std::string& string, const char *fontName, float fontSize);
    
    /** creates a LabelTTF from a fontname, horizontal alignment, dimension in points,  and font size in points.
     @since v2.0.1
     */
    static LabelTTF * create(const std::string& string, const char *fontName, float fontSize,
                               const Size& dimensions, CCTextAlignment hAlignment);
  
    /** creates a CCLabel from a fontname, alignment, dimension in points and font size in points
     @since v2.0.1
     */
    static LabelTTF * create(const std::string& string, const char *fontName, float fontSize,
                               const Size& dimensions, CCTextAlignment hAlignment, 
                               CCVerticalTextAlignment vAlignment);
    
    
    /** Create a lable with string and a font definition*/
    static LabelTTF * createWithFontDefinition(const std::string& string, axFontDefinition &textDefinition);
    
    /** initializes the LabelTTF with a font name and font size */
    bool initWithString(const std::string& string, const char *fontName, float fontSize);
    
    /** initializes the LabelTTF with a font name, alignment, dimension and font size */
    bool initWithString(const std::string& string, const char *fontName, float fontSize,
                        const Size& dimensions, CCTextAlignment hAlignment);

    /** initializes the LabelTTF with a font name, alignment, dimension and font size */
    bool initWithString(const std::string& string, const char *fontName, float fontSize,
                        const Size& dimensions, CCTextAlignment hAlignment, 
                        CCVerticalTextAlignment vAlignment);
    
    /** initializes the LabelTTF with a font name, alignment, dimension and font size */
    bool initWithStringAndTextDefinition(const std::string& string, axFontDefinition &textDefinition);
    
    /** set the text definition used by this label */
    void setTextDefinition(axFontDefinition *theDefinition);
    
    /** get the text definition used by this label */
    axFontDefinition * getTextDefinition();
    
    
    
    /** enable or disable shadow for the label */
    void enableShadow(const Size &shadowOffset, float shadowOpacity, float shadowBlur, bool mustUpdateTexture = true);
    
    /** disable shadow rendering */
    void disableShadow(bool mustUpdateTexture = true);
    
    /** enable or disable stroke */
    void enableStroke(const ccColor3B &strokeColor, float strokeSize, bool mustUpdateTexture = true);
    
    /** disable stroke */
    void disableStroke(bool mustUpdateTexture = true);
    
    /** set text tinting */
    void setFontFillColor(const ccColor3B &tintColor, bool mustUpdateTexture = true);

    
    
    /** initializes the LabelTTF */
    bool init();

    /** Creates an label.
     */
    static LabelTTF * create();

    /** changes the string to render
    * @warning Changing the string is as expensive as creating a new LabelTTF. To obtain better performance use LabelAtlas
    */
    virtual void setString(const std::string& label);
    virtual const std::string& getString(void);
    
    CCTextAlignment getHorizontalAlignment();
    void setHorizontalAlignment(CCTextAlignment alignment);
    
    CCVerticalTextAlignment getVerticalAlignment();
    void setVerticalAlignment(CCVerticalTextAlignment verticalAlignment);
    
    Size getDimensions();
    void setDimensions(const Size &dim);
    
    float getFontSize();
    void setFontSize(float fontSize);
    
    const char* getFontName();
    void setFontName(const char *fontName);
    
private:
    bool updateTexture();
protected:
    
    /** set the text definition for this label */
    void                _updateWithTextDefinition(axFontDefinition & textDefinition, bool mustUpdateTexture = true);
    axFontDefinition    _prepareTextDefinition(bool adjustForResolution = false);
    
    /** Dimensions of the label in Points */
    Size m_tDimensions;
    /** The alignment of the label */
    CCTextAlignment         m_hAlignment;
    /** The vertical alignment of the label */
    CCVerticalTextAlignment m_vAlignment;
    /** Font name used in the label */
    std::string * m_pFontName;
    /** Font size of the label */
    float m_fFontSize;
    /** label's string */
    std::string m_string;
    
    /** font shadow */
    bool    m_shadowEnabled;
    Size  m_shadowOffset;
    float   m_shadowOpacity;
    float   m_shadowBlur;
    
    
    /** font stroke */
    bool        m_strokeEnabled;
    ccColor3B   m_strokeColor;
    float       m_strokeSize;
        
    /** font tint */
    ccColor3B   m_textFillColor;

    
};


// end of GUI group
/// @}
/// @}

NS_AX_END

#endif //__CCLABEL_H__

