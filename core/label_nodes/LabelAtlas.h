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
****************************************************************************/
#ifndef __AXLABEL_ATLAS_H__
#define __AXLABEL_ATLAS_H__

#include "base/AtlasNode.h"

NS_AX_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup label
 * @{
 */

/** @brief LabelAtlas is a subclass of AtlasNode.

It can be as a replacement of CCLabel since it is MUCH faster.

LabelAtlas versus CCLabel:
- LabelAtlas is MUCH faster than CCLabel
- LabelAtlas "characters" have a fixed height and width
- LabelAtlas "characters" can be anything you want since they are taken from an image file

A more flexible class is LabelBMFont. It supports variable width characters and it also has a nice editor.
*/
class AX_DLL LabelAtlas : public AtlasNode, public LabelProtocol
{
public:
    /**
     *  @js ctor
     *  @lua NA
     */
    LabelAtlas()
        :m_sString("")
    {}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~LabelAtlas()
    { 
        m_sString.clear(); 
    }

    /** creates the LabelAtlas with a string, a char map file(the atlas), the width and height of each element and the starting char of the atlas */
    static LabelAtlas * create(const char *string, const char *charMapFile, unsigned int itemWidth, unsigned int itemHeight, unsigned int startCharMap);
    
    /** creates the LabelAtlas with a string and a configuration file
     @since v2.0
     @js _create
     */
    static LabelAtlas* create(const char *string, const char *fntFile);

    /** initializes the LabelAtlas with a string, a char map file(the atlas), the width and height of each element and the starting char of the atlas */
    bool initWithString(const char *string, const char *charMapFile, unsigned int itemWidth, unsigned int itemHeight, unsigned int startCharMap);
    
    /** initializes the LabelAtlas with a string and a configuration file
     @since v2.0
     */
    bool initWithString(const char *string, const char *fntFile);
    
    /** initializes the LabelAtlas with a string, a texture, the width and height in points of each element and the starting char of the atlas */
    bool initWithString(const char* string, Texture2D* texture, unsigned int itemWidth, unsigned int itemHeight, unsigned int startCharMap);
    
    // super methods
    virtual void updateAtlasValues();
    virtual void setString(const std::string& label);
    virtual const std::string& getString(void);
    
#if AX_LABELATLAS_DEBUG_DRAW
    virtual void draw();
#endif

protected:
    // string to render
    std::string m_sString;
    // the first char in the charmap
    unsigned int m_uMapStartChar;
};

// end of GUI group
/// @}
/// @}


NS_AX_END

#endif //__AXLABEL_ATLAS_H__
