/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2011 Ricardo Quesada
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

#ifndef __AXMENU_ITEM_H__
#define __AXMENU_ITEM_H__

#include "base/Node.h"
#include "Protocols.h"
#include "base/Array.h"

NS_AX_BEGIN

class LabelTTF;
class LabelAtlas;
class Sprite;
class SpriteFrame;
#define kAXItemSize 32

class AX_DLL MenuItem : public NodeRGBA
{
protected:
    /** whether or not the item is selected
     @since v0.8.2
     */
    bool m_bSelected;
    bool m_bEnabled;

public:
    /**
     *  @js ctor
     */
    MenuItem()
        : m_bSelected(false)
        , m_bEnabled(false)
        , m_nScriptTapHandler(0)
    {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItem();

    /** Creates a CCMenuItem with no target/selector
     * @js NA
     * @lua NA
     */
    static MenuItem* create();
    static MenuItem* create(axMenuCallback callback);

    bool initWithCallback(axMenuCallback callback);
    /** Returns the outside box */
    Rect rect();
    /** Activate the item */
    virtual void activate();
    /** The item was selected (not activated), similar to "mouse-over" */
    virtual void selected();
    /** The item was unselected */
    virtual void unselected();

    virtual bool isEnabled();
    //@note: It's 'setIsEnable' in axolotl-iphone. 
    virtual void setEnabled(bool value);
    virtual bool isSelected();

    /** set the target/selector of the menu item*/
    void setCallback(axMenuCallback callback);
protected:
    axMenuCallback _callback;
    int             m_nScriptTapHandler;
};

/** @brief An abstract class for "label" CCMenuItemLabel items
 Any CCNode that supports the CCLabelProtocol protocol can be added.
 Supported nodes:
 - CCBitmapFontAtlas
 - CCLabelAtlas
 - CCLabelTTF
 */
class AX_DLL MenuItemLabel : public MenuItem
{
    /** the color that will be used to disable the item */
   AX_PROPERTY_PASS_BY_REF(ccColor3B, m_tDisabledColor, DisabledColor);
    /** Label that is rendered. It can be any CCNode that implements the CCLabelProtocol */
   AX_PROPERTY(Node*, m_pLabel, Label);
public:
    /**
     *  @js ctor
     */
    MenuItemLabel()
        : m_pLabel(NULL)
        , m_fOriginalScale(0.0)
    {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItemLabel();

    /** creates a CCMenuItemLabel with a Label, target and selector
     * @lua NA
     */
    static MenuItemLabel* create(Node* label, axMenuCallback callback);
    /** creates a CCMenuItemLabel with a Label. Target and selector will be nil */
    static MenuItemLabel* create(Node* label);

    /** initializes a CCMenuItemLabel with a Label, target and selector */
    bool initWithLabel(Node* label, axMenuCallback callback);
    /** sets a new string to the inner label */
    void setString(const char* label);
    // super methods
    virtual void activate();
    virtual void selected();
    virtual void unselected();
    /** Enable or disabled the CCMenuItemFont
     @warning setEnabled changes the RGB color of the font
     */
    virtual void setEnabled(bool enabled);

protected:
    ccColor3B    m_tColorBackup;
    float        m_fOriginalScale;
};


/** @brief A CCMenuItemAtlasFont
 Helper class that creates a MenuItemLabel class with a LabelAtlas
 */
class AX_DLL MenuItemAtlasFont : public MenuItemLabel
{
public:
    /**
     *  @js ctor
     */
    MenuItemAtlasFont() {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItemAtlasFont() {}

    /** creates a menu item from a string and atlas with a target/selector */
    static MenuItemAtlasFont* create(const char* value, const char* charMapFile, int itemWidth, int itemHeight, char startCharMap);
    /** creates a menu item from a string and atlas. Use it with MenuItemToggle
     * @lua NA
     */
    static MenuItemAtlasFont* create(const char* value, const char* charMapFile, int itemWidth, int itemHeight, char startCharMap, axMenuCallback callback);
    /** initializes a menu item from a string and atlas with a target/selector */
    bool initWithString(const char* value, const char* charMapFile, int itemWidth, int itemHeight, char startCharMap, axMenuCallback callback);
};


/** @brief A CCMenuItemFont
 Helper class that creates a CCMenuItemLabel class with a Label
 */
class AX_DLL MenuItemFont : public MenuItemLabel
{
public:
    /**
     *  @js ctor
     */
    MenuItemFont() : m_uFontSize(0), m_strFontName("") {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItemFont() {}
    /** set default font size */
    static void setFontSize(unsigned int s);
    /** get default font size */
    static unsigned int fontSize();
    /** set the default font name */
    static void setFontName(const char* name);
    /** get the default font name */
    static const char* fontName();

    /** creates a menu item from a string without target/selector. To be used with CCMenuItemToggle */
    static MenuItemFont* create(const char* value);
    /** creates a menu item from a string with a target/selector
     * @lua NA
     */
    static MenuItemFont* create(const char* value, axMenuCallback callback);

    /** initializes a menu item from a string with a target/selector */
    bool initWithString(const char* value, axMenuCallback callback);

    /** set font size
     * c++ can not overload static and non-static member functions with the same parameter types
     * so change the name to setFontSizeObj
     * @js setFontSize
     */
    void setFontSizeObj(unsigned int s);

    /** get font size
     *  @js fontSize
     */
    unsigned int fontSizeObj();

    /** set the font name
     * c++ can not overload static and non-static member functions with the same parameter types
     * so change the name to setFontNameObj
     * @js setFontName
     */
    void setFontNameObj(const char* name);
    /**
     *  @js fontName
     */
    const char* fontNameObj();

protected:
    void recreateLabel();

    unsigned int m_uFontSize;
    std::string m_strFontName;
};


/** @brief CCMenuItemSprite accepts CCNode<CCRGBAProtocol> objects as items.
 The images has 3 different states:
 - unselected image
 - selected image
 - disabled image

 @since v0.8.0
 */
class AX_DLL MenuItemSprite : public MenuItem
{
    /** the image used when the item is not selected */
   AX_PROPERTY(Node*, m_pNormalImage, NormalImage);
    /** the image used when the item is selected */
   AX_PROPERTY(Node*, m_pSelectedImage, SelectedImage);
    /** the image used when the item is disabled */
   AX_PROPERTY(Node*, m_pDisabledImage, DisabledImage);
public:
    /**
     *  @js ctor
     */
    MenuItemSprite()
        :m_pNormalImage(NULL)
        , m_pSelectedImage(NULL)
        , m_pDisabledImage(NULL)
    {}

    /** creates a menu item with a normal, selected and disabled image*/
    static MenuItemSprite* create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite = NULL);
    /** creates a menu item with a normal and selected image with target/selector
     * @lua NA
     */
    static MenuItemSprite* create(Node* normalSprite, Node* selectedSprite, axMenuCallback callback);
    /** creates a menu item with a normal,selected  and disabled image with target/selector
     * @lua NA
     */
    static MenuItemSprite* create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, axMenuCallback callback);

    /** initializes a menu item with a normal, selected  and disabled image with target/selector */
    bool initWithNormalSprite(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, axMenuCallback callback);

    /**
     @since v0.99.5
     */
    virtual void selected();
    virtual void unselected();
    virtual void setEnabled(bool bEnabled);

protected:
    virtual void updateImagesVisibility();
};


/** @brief CCMenuItemImage accepts images as items.
 The images has 3 different states:
 - unselected image
 - selected image
 - disabled image

 For best results try that all images are of the same size
 */
class AX_DLL MenuItemImage : public MenuItemSprite
{
public:
    /**
     * @js ctor
     * @lua NA
     */
    MenuItemImage() {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItemImage() {}

    /** creates a menu item with a normal and selected image*/
    static MenuItemImage* create(const char* normalImage, const char* selectedImage);
    /** creates a menu item with a normal,selected  and disabled image*/
    static MenuItemImage* create(const char* normalImage, const char* selectedImage, const char* disabledImage);
    /** creates a menu item with a normal and selected image with target/selector
     * @lua NA
     */
    static MenuItemImage* create(const char* normalImage, const char* selectedImage, axMenuCallback callback);
    /** creates a menu item with a normal,selected  and disabled image with target/selector
     * @lua NA
     */
    static MenuItemImage* create(const char* normalImage, const char* selectedImage, const char* disabledImage, axMenuCallback callback);

    bool init();
    /** initializes a menu item with a normal, selected  and disabled image with target/selector */
    bool initWithNormalImage(const char* normalImage, const char* selectedImage, const char* disabledImage, axMenuCallback callback);
    /** sets the sprite frame for the normal image */
    void setNormalSpriteFrame(SpriteFrame* frame);
    /** sets the sprite frame for the selected image */
    void setSelectedSpriteFrame(SpriteFrame* frame);
    /** sets the sprite frame for the disabled image */
    void setDisabledSpriteFrame(SpriteFrame* frame);

    /** Creates an CCMenuItemImage.
     * @js NA
     */
    static MenuItemImage* create();
};


/** @brief A CCMenuItemToggle
 A simple container class that "toggles" it's inner items
 The inner items can be any MenuItem
 */
class AX_DLL MenuItemToggle : public MenuItem
{
    /** returns the selected item */
   AX_PROPERTY(unsigned int, m_uSelectedIndex, SelectedIndex);
    /** CCMutableArray that contains the subitems. You can add/remove items in runtime, and you can replace the array with a new one.
     @since v0.7.2
     */
   AX_PROPERTY(Array*, m_pSubItems, SubItems);
public:
    /**
     * @js ctor
     */
    MenuItemToggle()
        : m_uSelectedIndex(0)
        , m_pSubItems(NULL)
    {}
    /**
     * @js NA
     * @lua NA
     */
    virtual ~MenuItemToggle();

    /** creates a menu item from a CCArray with a target selector */
    static MenuItemToggle* createWithTarget(axMenuCallback callback, Array* menuItems);

    /** creates a menu item from a list of items with a target/selector */
    static MenuItemToggle* createWithTarget(axMenuCallback callback, MenuItem* item, ...);

    /** creates a menu item with no target/selector and no items
     * @js NA
     * @lua NA
     */
    static MenuItemToggle* create();

    /** initializes a menu item from a list of items with a target selector */
    bool initWithTarget(axMenuCallback callback, MenuItem* item, va_list args);

    /** creates a menu item with a item */
    static MenuItemToggle* create(MenuItem* item);

    /** initializes a menu item with a item */
    bool initWithItem(MenuItem* item);
    /** add more menu item */
    void addSubItem(MenuItem* item);

    /** return the selected item */
    MenuItem* selectedItem();
    // super methods
    virtual void activate();
    virtual void selected();
    virtual void unselected();
    virtual void setEnabled(bool var);

};


// end of GUI group
/// @}
/// @}

NS_AX_END

#endif //__AXMENU_ITEM_H__
