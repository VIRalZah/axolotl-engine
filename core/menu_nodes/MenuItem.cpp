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

#include "MenuItem.h"
#include "support/PointExtension.h"
#include "actions/ActionInterval.h"
#include "sprite_nodes/Sprite.h"
#include "label_nodes/LabelAtlas.h"
#include "label_nodes/LabelTTF.h"
#include <stdarg.h>
#include <cstring>

NS_AX_BEGIN

static unsigned int _globalFontSize = kAXItemSize;
static std::string _globalFontName = "Marker Felt";
static bool _globalFontNameRelease = false;

const unsigned int    kCurrentItem = 0xc0c05001;
const unsigned int    kZoomActionTag = 0xc0c05002;

const unsigned int    kNormalTag = 0x1;
const unsigned int    kSelectedTag = 0x2;
const unsigned int    kDisableTag = 0x3;

//
// MenuItem
//

MenuItem* MenuItem::create()
{
    return MenuItem::create(nullptr);
}

MenuItem* MenuItem::create(axMenuCallback callback)
{
    auto ret = new MenuItem();
    if (ret && ret->initWithCallback(callback))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool MenuItem::initWithCallback(axMenuCallback callback)
{
    setAnchorPoint(Vec2(0.5f, 0.5f));
    _callback = callback;
    m_bEnabled = true;
    m_bSelected = false;
    return true;
}

MenuItem::~MenuItem()
{
}

void MenuItem::selected()
{
    m_bSelected = true;
}

void MenuItem::unselected()
{
    m_bSelected = false;
}

void MenuItem::activate()
{
    if (m_bEnabled)
    {
        if (_callback)
        {
            _callback(this);
        }
    }
}

void MenuItem::setEnabled(bool enabled)
{
    m_bEnabled = enabled;
}

bool MenuItem::isEnabled()
{
    return m_bEnabled;
}

Rect MenuItem::rect()
{
    return Rect(m_obPosition.x - m_obContentSize.width * m_obAnchorPoint.x,
        m_obPosition.y - m_obContentSize.height * m_obAnchorPoint.y,
        m_obContentSize.width, m_obContentSize.height);
}

bool MenuItem::isSelected()
{
    return m_bSelected;
}

void MenuItem::setCallback(axMenuCallback callback)
{
    _callback = callback;
}

//
//MenuItemLabel
//

const ccColor3B& MenuItemLabel::getDisabledColor()
{
    return m_tDisabledColor;
}
void MenuItemLabel::setDisabledColor(const ccColor3B& var)
{
    m_tDisabledColor = var;
}

Node* MenuItemLabel::getLabel()
{
    return m_pLabel;
}
void MenuItemLabel::setLabel(Node* var)
{
    if (var)
    {
        addChild(var);
        var->setAnchorPoint(Vec2(0, 0));
        setContentSize(var->getContentSize());
    }

    if (m_pLabel)
    {
        removeChild(m_pLabel, true);
    }

    m_pLabel = var;
}

MenuItemLabel* MenuItemLabel::create(Node* label, axMenuCallback callback)
{
    MenuItemLabel* pRet = new MenuItemLabel();
    pRet->initWithLabel(label, callback);
    pRet->autorelease();
    return pRet;
}

MenuItemLabel* MenuItemLabel::create(Node* label)
{
    MenuItemLabel* pRet = new MenuItemLabel();
    pRet->initWithLabel(label, nullptr);
    pRet->autorelease();
    return pRet;
}

bool MenuItemLabel::initWithLabel(Node* label, axMenuCallback callback)
{
    MenuItem::initWithCallback(callback);

    m_fOriginalScale = 1.0f;
    m_tColorBackup = ccWHITE;
    setDisabledColor(ccc3(126, 126, 126));
    this->setLabel(label);

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

MenuItemLabel::~MenuItemLabel()
{
}

void MenuItemLabel::setString(const char* label)
{
    dynamic_cast<LabelProtocol*>(m_pLabel)->setString(label);
    this->setContentSize(m_pLabel->getContentSize());
}

void MenuItemLabel::activate()
{
    if (m_bEnabled)
    {
        this->stopAllActions();
        this->setScale(m_fOriginalScale);
        MenuItem::activate();
    }
}

void MenuItemLabel::selected()
{
    // subclass to change the default action
    if (m_bEnabled)
    {
        MenuItem::selected();

        Action* action = getActionByTag(kZoomActionTag);
        if (action)
        {
            this->stopAction(action);
        }
        else
        {
            m_fOriginalScale = this->getScale();
        }

        Action* zoomAction = ScaleTo::create(0.1f, m_fOriginalScale * 1.2f);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void MenuItemLabel::unselected()
{
    // subclass to change the default action
    if (m_bEnabled)
    {
        MenuItem::unselected();
        this->stopActionByTag(kZoomActionTag);
        Action* zoomAction = ScaleTo::create(0.1f, m_fOriginalScale);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void MenuItemLabel::setEnabled(bool enabled)
{
    if (m_bEnabled != enabled)
    {
        if (enabled == false)
        {
            m_tColorBackup = dynamic_cast<RGBAProtocol*>(m_pLabel)->getColor();
            dynamic_cast<RGBAProtocol*>(m_pLabel)->setColor(m_tDisabledColor);
        }
        else
        {
            dynamic_cast<RGBAProtocol*>(m_pLabel)->setColor(m_tColorBackup);
        }
    }
    MenuItem::setEnabled(enabled);
}

//
//MenuItemAtlasFont
//

MenuItemAtlasFont* MenuItemAtlasFont::create(const char* value, const char* charMapFile, int itemWidth, int itemHeight, char startCharMap)
{
    return MenuItemAtlasFont::create(value, charMapFile, itemWidth, itemHeight, startCharMap, nullptr);
}

MenuItemAtlasFont* MenuItemAtlasFont::create(const char* value, const char* charMapFile, int itemWidth, int itemHeight, char startCharMap, axMenuCallback callback)
{
    MenuItemAtlasFont* pRet = new MenuItemAtlasFont();
    pRet->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap, callback);
    pRet->autorelease();
    return pRet;
}

bool MenuItemAtlasFont::initWithString(const char* value, const char* charMapFile, int itemWidth, int itemHeight, char startCharMap, axMenuCallback callback)
{
    AXAssert(value != NULL && strlen(value) != 0, "value length must be greater than 0");
    LabelAtlas* label = new LabelAtlas();
    label->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap);
    label->autorelease();
    if (MenuItemLabel::initWithLabel(label, callback))
    {
        // do something ?
    }
    return true;
}
//
//CCMenuItemFont
//

void MenuItemFont::setFontSize(unsigned int s)
{
    _globalFontSize = s;
}

unsigned int MenuItemFont::fontSize()
{
    return _globalFontSize;
}

void MenuItemFont::setFontName(const char* name)
{
    if (_globalFontNameRelease)
    {
        _globalFontName.clear();
    }
    _globalFontName = name;
    _globalFontNameRelease = true;
}

const char* MenuItemFont::fontName()
{
    return _globalFontName.c_str();
}

MenuItemFont* MenuItemFont::create(const char* value, axMenuCallback callback)
{
    MenuItemFont* pRet = new MenuItemFont();
    pRet->initWithString(value, callback);
    pRet->autorelease();
    return pRet;
}

MenuItemFont* MenuItemFont::create(const char* value)
{
    MenuItemFont* pRet = new MenuItemFont();
    pRet->initWithString(value, nullptr);
    pRet->autorelease();
    return pRet;
}

bool MenuItemFont::initWithString(const char* value, axMenuCallback callback)
{
    AXAssert(value != NULL && strlen(value) != 0, "Value length must be greater than 0");

    m_strFontName = _globalFontName;
    m_uFontSize = _globalFontSize;

    LabelTTF* label = LabelTTF::create(value, m_strFontName.c_str(), (float)m_uFontSize);
    if (MenuItemLabel::initWithLabel(label, callback))
    {
        // do something ?
    }
    return true;
}

void MenuItemFont::recreateLabel()
{
    LabelTTF* label = LabelTTF::create(dynamic_cast<LabelProtocol*>(m_pLabel)->getString(),
        m_strFontName.c_str(), (float)m_uFontSize);
    this->setLabel(label);
}

void MenuItemFont::setFontSizeObj(unsigned int s)
{
    m_uFontSize = s;
    recreateLabel();
}

unsigned int MenuItemFont::fontSizeObj()
{
    return m_uFontSize;
}

void MenuItemFont::setFontNameObj(const char* name)
{
    m_strFontName = name;
    recreateLabel();
}

const char* MenuItemFont::fontNameObj()
{
    return m_strFontName.c_str();
}

//
//MenuItemSprite
//

Node* MenuItemSprite::getNormalImage()
{
    return m_pNormalImage;
}

void MenuItemSprite::setNormalImage(Node* pImage)
{
    if (pImage != m_pNormalImage)
    {
        if (pImage)
        {
            addChild(pImage, 0, kNormalTag);
            pImage->setAnchorPoint(Vec2(0, 0));
        }

        if (m_pNormalImage)
        {
            removeChild(m_pNormalImage, true);
        }

        m_pNormalImage = pImage;
        this->setContentSize(m_pNormalImage->getContentSize());
        this->updateImagesVisibility();
    }
}

Node* MenuItemSprite::getSelectedImage()
{
    return m_pSelectedImage;
}

void MenuItemSprite::setSelectedImage(Node* pImage)
{
    if (pImage != m_pNormalImage)
    {
        if (pImage)
        {
            addChild(pImage, 0, kSelectedTag);
            pImage->setAnchorPoint(Vec2(0, 0));
        }

        if (m_pSelectedImage)
        {
            removeChild(m_pSelectedImage, true);
        }

        m_pSelectedImage = pImage;
        this->updateImagesVisibility();
    }
}

Node* MenuItemSprite::getDisabledImage()
{
    return m_pDisabledImage;
}

void MenuItemSprite::setDisabledImage(Node* pImage)
{
    if (pImage != m_pNormalImage)
    {
        if (pImage)
        {
            addChild(pImage, 0, kDisableTag);
            pImage->setAnchorPoint(Vec2(0, 0));
        }

        if (m_pDisabledImage)
        {
            removeChild(m_pDisabledImage, true);
        }

        m_pDisabledImage = pImage;
        this->updateImagesVisibility();
    }
}

//
//MenuItemSprite
//

MenuItemSprite* MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite)
{
    return MenuItemSprite::create(normalSprite, selectedSprite, disabledSprite, nullptr);
}

MenuItemSprite* MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, axMenuCallback callback)
{
    return MenuItemSprite::create(normalSprite, selectedSprite, nullptr, callback);
}

MenuItemSprite* MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, axMenuCallback callback)
{
    MenuItemSprite* pRet = new MenuItemSprite();
    pRet->initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
    pRet->autorelease();
    return pRet;
}

bool MenuItemSprite::initWithNormalSprite(Node* normalSprite, Node* selectedSprite, Node* disabledSprite, axMenuCallback callback)
{
    MenuItem::initWithCallback(callback);
    setNormalImage(normalSprite);
    setSelectedImage(selectedSprite);
    setDisabledImage(disabledSprite);

    if (m_pNormalImage)
    {
        this->setContentSize(m_pNormalImage->getContentSize());
    }

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

/**
 @since v0.99.5
 */
void MenuItemSprite::selected()
{
    MenuItem::selected();

    if (m_pNormalImage)
    {
        if (m_pDisabledImage)
        {
            m_pDisabledImage->setVisible(false);
        }

        if (m_pSelectedImage)
        {
            m_pNormalImage->setVisible(false);
            m_pSelectedImage->setVisible(true);
        }
        else
        {
            m_pNormalImage->setVisible(true);
        }
    }
}

void MenuItemSprite::unselected()
{
    MenuItem::unselected();
    if (m_pNormalImage)
    {
        m_pNormalImage->setVisible(true);

        if (m_pSelectedImage)
        {
            m_pSelectedImage->setVisible(false);
        }

        if (m_pDisabledImage)
        {
            m_pDisabledImage->setVisible(false);
        }
    }
}

void MenuItemSprite::setEnabled(bool bEnabled)
{
    if (m_bEnabled != bEnabled)
    {
        MenuItem::setEnabled(bEnabled);
        this->updateImagesVisibility();
    }
}

// Helper 
void MenuItemSprite::updateImagesVisibility()
{
    if (m_bEnabled)
    {
        if (m_pNormalImage)   m_pNormalImage->setVisible(true);
        if (m_pSelectedImage) m_pSelectedImage->setVisible(false);
        if (m_pDisabledImage) m_pDisabledImage->setVisible(false);
    }
    else
    {
        if (m_pDisabledImage)
        {
            if (m_pNormalImage)   m_pNormalImage->setVisible(false);
            if (m_pSelectedImage) m_pSelectedImage->setVisible(false);
            if (m_pDisabledImage) m_pDisabledImage->setVisible(true);
        }
        else
        {
            if (m_pNormalImage)   m_pNormalImage->setVisible(true);
            if (m_pSelectedImage) m_pSelectedImage->setVisible(false);
            if (m_pDisabledImage) m_pDisabledImage->setVisible(false);
        }
    }
}

///
/// CCMenuItemImage
///

MenuItemImage* MenuItemImage::create()
{
    MenuItemImage* pRet = new MenuItemImage();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool MenuItemImage::init(void)
{
    return initWithNormalImage(nullptr, nullptr, nullptr, nullptr);
}

MenuItemImage* MenuItemImage::create(const char* normalImage, const char* selectedImage)
{
    return MenuItemImage::create(normalImage, selectedImage, nullptr, nullptr);
}

MenuItemImage* MenuItemImage::create(const char* normalImage, const char* selectedImage, axMenuCallback callback)
{
    return MenuItemImage::create(normalImage, selectedImage, nullptr, callback);
}

MenuItemImage* MenuItemImage::create(const char* normalImage, const char* selectedImage, const char* disabledImage, axMenuCallback callback)
{
    MenuItemImage* pRet = new MenuItemImage();
    if (pRet && pRet->initWithNormalImage(normalImage, selectedImage, disabledImage, callback))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

MenuItemImage* MenuItemImage::create(const char* normalImage, const char* selectedImage, const char* disabledImage)
{
    MenuItemImage* pRet = new MenuItemImage();
    if (pRet && pRet->initWithNormalImage(normalImage, selectedImage, disabledImage, nullptr))
    {
        pRet->autorelease();
        return pRet;
    }
    AX_SAFE_DELETE(pRet);
    return NULL;
}

bool MenuItemImage::initWithNormalImage(const char* normalImage, const char* selectedImage, const char* disabledImage, axMenuCallback callback)
{
    Node* normalSprite = NULL;
    Node* selectedSprite = NULL;
    Node* disabledSprite = NULL;

    if (normalImage)
    {
        normalSprite = Sprite::create(normalImage);
    }

    if (selectedImage)
    {
        selectedSprite = Sprite::create(selectedImage);
    }

    if (disabledImage)
    {
        disabledSprite = Sprite::create(disabledImage);
    }
    return initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
}
//
// Setter of sprite frames
//
void MenuItemImage::setNormalSpriteFrame(SpriteFrame* frame)
{
    setNormalImage(Sprite::createWithSpriteFrame(frame));
}

void MenuItemImage::setSelectedSpriteFrame(SpriteFrame* frame)
{
    setSelectedImage(Sprite::createWithSpriteFrame(frame));
}

void MenuItemImage::setDisabledSpriteFrame(SpriteFrame* frame)
{
    setDisabledImage(Sprite::createWithSpriteFrame(frame));
}

//
// MenuItemToggle
//

void MenuItemToggle::setSubItems(Array* var)
{
    AX_SAFE_RETAIN(var);
    AX_SAFE_RELEASE(m_pSubItems);
    m_pSubItems = var;
}

Array* MenuItemToggle::getSubItems()
{
    return m_pSubItems;
}

MenuItemToggle* MenuItemToggle::createWithTarget(axMenuCallback callback, Array* menuItems)
{
    MenuItemToggle* pRet = new MenuItemToggle();
    pRet->MenuItem::initWithCallback(callback);
    pRet->m_pSubItems = Array::create();
    pRet->m_pSubItems->retain();

    for (unsigned int z = 0; z < menuItems->count(); z++)
    {
        MenuItem* menuItem = (MenuItem*)menuItems->objectAtIndex(z);
        pRet->m_pSubItems->addObject(menuItem);
    }

    pRet->m_uSelectedIndex = UINT_MAX;
    pRet->setSelectedIndex(0);
    return pRet;
}

MenuItemToggle* MenuItemToggle::createWithTarget(axMenuCallback callback, MenuItem* item, ...)
{
    va_list args;
    va_start(args, item);
    MenuItemToggle* pRet = new MenuItemToggle();
    pRet->initWithTarget(callback, item, args);
    pRet->autorelease();
    va_end(args);
    return pRet;
}

MenuItemToggle* MenuItemToggle::create()
{
    MenuItemToggle* pRet = new MenuItemToggle();
    pRet->initWithItem(NULL);
    pRet->autorelease();
    return pRet;
}

bool MenuItemToggle::initWithTarget(axMenuCallback callback, MenuItem* item, va_list args)
{
    MenuItem::initWithCallback(callback);
    this->m_pSubItems = Array::create();
    this->m_pSubItems->retain();
    int z = 0;
    MenuItem* i = item;
    while (i)
    {
        z++;
        m_pSubItems->addObject(i);
        i = va_arg(args, MenuItem*);
    }
    m_uSelectedIndex = UINT_MAX;
    this->setSelectedIndex(0);
    return true;
}

MenuItemToggle* MenuItemToggle::create(MenuItem* item)
{
    MenuItemToggle* pRet = new MenuItemToggle();
    pRet->initWithItem(item);
    pRet->autorelease();
    return pRet;
}

bool MenuItemToggle::initWithItem(MenuItem* item)
{
    MenuItem::initWithCallback(nullptr);
    setSubItems(Array::create());

    if (item)
    {
        m_pSubItems->addObject(item);
    }
    m_uSelectedIndex = UINT_MAX;
    this->setSelectedIndex(0);

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

void MenuItemToggle::addSubItem(MenuItem* item)
{
    m_pSubItems->addObject(item);
}

MenuItemToggle::~MenuItemToggle()
{
    AX_SAFE_RELEASE(m_pSubItems);
}
void MenuItemToggle::setSelectedIndex(unsigned int index)
{
    if (index != m_uSelectedIndex && m_pSubItems->count() > 0)
    {
        m_uSelectedIndex = index;
        MenuItem* currentItem = (MenuItem*)getChildByTag(kCurrentItem);
        if (currentItem)
        {
            currentItem->removeFromParentAndCleanup(false);
        }

        MenuItem* item = (MenuItem*)m_pSubItems->objectAtIndex(m_uSelectedIndex);
        this->addChild(item, 0, kCurrentItem);
        Size s = item->getContentSize();
        this->setContentSize(s);
        item->setPosition(Vec2(s.width / 2, s.height / 2));
    }
}
unsigned int MenuItemToggle::getSelectedIndex()
{
    return m_uSelectedIndex;
}
void MenuItemToggle::selected()
{
    MenuItem::selected();
    ((MenuItem*)(m_pSubItems->objectAtIndex(m_uSelectedIndex)))->selected();
}
void MenuItemToggle::unselected()
{
    MenuItem::unselected();
    ((MenuItem*)(m_pSubItems->objectAtIndex(m_uSelectedIndex)))->unselected();
}
void MenuItemToggle::activate()
{
    // update index
    if (m_bEnabled)
    {
        unsigned int newIndex = (m_uSelectedIndex + 1) % m_pSubItems->count();
        this->setSelectedIndex(newIndex);
    }
    MenuItem::activate();
}
void MenuItemToggle::setEnabled(bool enabled)
{
    if (m_bEnabled != enabled)
    {
        MenuItem::setEnabled(enabled);

        if (m_pSubItems && m_pSubItems->count() > 0)
        {
            Object* pObj = NULL;
            AXARRAY_FOREACH(m_pSubItems, pObj)
            {
                MenuItem* pItem = (MenuItem*)pObj;
                pItem->setEnabled(enabled);
            }
        }
    }
}

MenuItem* MenuItemToggle::selectedItem()
{
    return (MenuItem*)m_pSubItems->objectAtIndex(m_uSelectedIndex);
}

NS_AX_END
