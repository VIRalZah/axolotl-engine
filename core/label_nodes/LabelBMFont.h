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
#ifndef __AXBITMAP_FONT_ATLAS_H__
#define __AXBITMAP_FONT_ATLAS_H__

#include "sprite_nodes/SpriteBatchNode.h"
#include "support/data_support/uthash.h"
#include <map>
#include <sstream>
#include <iostream>
#include <vector>
#include <set>
#include "base/BMFontCache.h"

NS_AX_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup label
 * @{
 */

enum {
    kCCLabelAutomaticWidth = -1,
};

/** @brief LabelBMFont is a subclass of SpriteBatchNode.

Features:
- Treats each character like a Sprite. This means that each individual character can be:
- rotated
- scaled
- translated
- tinted
- change the opacity
- It can be used as part of a menu item.
- anchorPoint can be used to align the "label"
- Supports AngelCode text format

Limitations:
- All inner characters are using an anchorPoint of (0.5f, 0.5f) and it is not recommend to change it
because it might affect the rendering

LabelBMFont implements the protocol LabelProtocol, like CCLabel and LabelAtlas.
LabelBMFont has the flexibility of CCLabel, the speed of LabelAtlas and all the features of Sprite.
If in doubt, use LabelBMFont instead of LabelAtlas / CCLabel.

Supported editors:
http://glyphdesigner.71squared.com/ (Commercial, Mac OS X)
http://www.n4te.com/hiero/hiero.jnlp (Free, Java)
http://slick.cokeandcode.com/demos/hiero.jnlp (Free, Java)
http://www.angelcode.com/products/bmfont/ (Free, Windows only)

@since v0.8
*/

class AX_DLL LabelBMFont : public SpriteBatchNode, public LabelProtocol, public RGBAProtocol
{
public:
    /**
     *  @js ctor
     */
    LabelBMFont();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~LabelBMFont();
    /** Purges the cached data.
    Removes from memory the cached configurations and the atlas name dictionary.
    @since v0.99.3
    */

    /** creates a bitmap font atlas with an initial string and the FNT file */
    static LabelBMFont * create(const char *str, const char *fntFile, float width, CCTextAlignment alignment, Vec2 imageOffset);
    
	static LabelBMFont * create(const char *str, const char *fntFile, float width, CCTextAlignment alignment);

	static LabelBMFont * create(const char *str, const char *fntFile, float width);

	static LabelBMFont * create(const char *str, const char *fntFile);

    /** Creates an label.
     */
    static LabelBMFont * create();

    bool init();
    /** init a bitmap font atlas with an initial string and the FNT file */
    bool initWithString(const char *str, const char *fntFile, float width = kCCLabelAutomaticWidth, CCTextAlignment alignment = kCCTextAlignmentLeft, Vec2 imageOffset = Vec2::ZERO);

    /** updates the font chars based on the string to render */
    void createFontChars();
    // super method
    virtual void setString(const std::string& newString);
    virtual void setString(const std::string& newString, bool needUpdateLabel);

    virtual const std::string& getString(void);
    virtual void setCString(const char *label);
    virtual void setAnchorPoint(const Vec2& var);
    virtual void updateLabel();
    virtual void setAlignment(CCTextAlignment alignment);
    virtual void setWidth(float width);
    virtual void setLineBreakWithoutSpace(bool breakWithoutSpace);
    virtual void setScale(float scale);
    virtual void setScaleX(float scaleX);
    virtual void setScaleY(float scaleY);

    virtual void limitLabelWidth(float maxContentSize, float minScale = 0.01f);
    
    // RGBAProtocol 
    virtual bool isOpacityModifyRGB();
    virtual void setOpacityModifyRGB(bool isOpacityModifyRGB); virtual GLubyte getOpacity();
    virtual GLubyte getDisplayedOpacity();
    virtual void setOpacity(GLubyte opacity);
    virtual void updateDisplayedOpacity(GLubyte parentOpacity);
    virtual bool isCascadeOpacityEnabled();
    virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
    virtual const ccColor3B& getColor(void);
    virtual const ccColor3B& getDisplayedColor();
    virtual void setColor(const ccColor3B& color);
    virtual void updateDisplayedColor(const ccColor3B& parentColor);
    virtual bool isCascadeColorEnabled();
    virtual void setCascadeColorEnabled(bool cascadeColorEnabled);

    void setFntFile(const char* fntFile);
    const char* getFntFile();
	BMFontConfiguration* getConfiguration() const;
#if AX_LABELBMFONT_DEBUG_DRAW
    virtual void draw();
#endif // AX_LABELBMFONT_DEBUG_DRAW
private:
    char * atlasNameFromFntFile(const char *fntFile);
    int kerningAmountForFirst(unsigned short first, unsigned short second);
    float getLetterPosXLeft( Sprite* characterSprite );
    float getLetterPosXRight( Sprite* characterSprite );
    
protected:
    virtual void setString(unsigned short *newString, bool needUpdateLabel);
    // string to render
    unsigned short* m_sString;
    
    // name of fntFile
    std::string m_sFntFile;
    
    // initial string without line breaks
    unsigned short* m_sInitialString;
    std::string m_sInitialStringUTF8;
    
    // alignment of all lines
    CCTextAlignment m_pAlignment;
    // max width until a line break is added
    float m_fWidth;
    
    BMFontConfiguration *m_pConfiguration;
    
    bool m_bLineBreakWithoutSpaces;
    // offset of the texture atlas
    Vec2    m_tImageOffset;
    
    // reused char
    Sprite *m_pReusedChar;
    
    // texture RGBA
    GLubyte m_cDisplayedOpacity;
    GLubyte m_cRealOpacity;
    ccColor3B m_tDisplayedColor;
    ccColor3B m_tRealColor;
    bool m_bCascadeColorEnabled;
    bool m_bCascadeOpacityEnabled;
    /** conforms to RGBAProtocol protocol */
    bool        m_bIsOpacityModifyRGB;

};

NS_AX_END

#endif //__AXBITMAP_FONT_ATLAS_H__
