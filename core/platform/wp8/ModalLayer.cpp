/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

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
#include "ModalLayer.h"

using namespace axolotl;

ModalLayer::~ModalLayer()
{
	AX_SAFE_RELEASE(m_frame);
}

bool ModalLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Director* pDirector = Director::sharedDirector();
	Size visibleSize = pDirector->getVisibleSize();
    Point origin = pDirector->getVisibleOrigin();

    pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
    
	m_frame = LayerColor::create(ccc4(0, 0, 0, 255 *.75));
	AX_SAFE_RETAIN(m_frame);

    m_frame->setPosition(Point(0, 0));
    this->addChild(m_frame);

	MenuItemFont *pCloseItem = MenuItemFont::create("OK", this, menu_selector(ModalLayer::menuCloseCallback) );
    Menu* pMenu = Menu::create(pCloseItem, NULL);
    pMenu->setPosition(Point(visibleSize.width/2, visibleSize.height/2));
    m_frame->addChild(pMenu);
    
    return true;
}

void ModalLayer::setMessage(const char* pszMsg)
{
    Director* pDirector = Director::sharedDirector();
	Size visibleSize = pDirector->getVisibleSize();
    Point origin = pDirector->getVisibleOrigin();

	LabelTTF *pLabel = LabelTTF::create(pszMsg,  "Arial", 24, CCSizeMake(0, 0), kCCTextAlignmentLeft, kCCVerticalTextAlignmentTop);
   pLabel->setPosition(Point(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

 
    // add the label as a child to this layer
    m_frame->addChild(pLabel, 1);
}





bool ModalLayer::ccTouchBegan(Touch* touch) {
    // can not touch on back layers
    return true;
}

void ModalLayer::menuCloseCallback(Object* pSender)
{
    this->removeFromParentAndCleanup(true);
    
    Director* pDirector = Director::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}