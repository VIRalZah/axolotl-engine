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
#ifndef __AXMENU_H_
#define __AXMENU_H_

#include "MenuItem.h"
#include "layers_scenes_transitions_nodes/Layer.h"

NS_AX_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup menu
 * @{
 */
typedef enum  
{
    kCCMenuStateWaiting,
    kCCMenuStateTrackingTouch
} tCCMenuState;

enum {
    //* priority used by the menu for the event handler
    kCCMenuHandlerPriority = -128,
};

/** @brief A Menu
* 
* Features and Limitation:
*  - You can add MenuItem objects in runtime using addChild:
*  - But the only accepted children are MenuItem objects
*/
class AX_DLL Menu : public LayerRGBA
{
    /** whether or not the menu will receive events */
    bool m_bEnabled;
    
public:
    /**
     *  @js ctor
     */
    Menu() : m_pSelectedItem(NULL) {}
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~Menu(){}

    /** creates an empty Menu */
    static Menu* create();

    /** creates a Menu with MenuItem objects 
     * @lua NA
     */
    static Menu* create(MenuItem* item, ...);

    /** creates a Menu with a Array of MenuItem objects 
     * @js NA
     */
    static Menu* createWithArray(Array* pArrayOfItems);

    /** creates a Menu with it's item, then use addChild() to add 
      * other items. It is used for script, it can't init with undetermined
      * number of variables.
      * @js NA
    */
    static Menu* createWithItem(MenuItem* item);
    
    /** creates a Menu with MenuItem objects 
     * @js NA
     * @lua NA
     */
    static Menu* createWithItems(MenuItem *firstItem, va_list args);

    /** initializes an empty Menu */
    bool init();

    /** initializes a Menu with a NSArray of MenuItem objects 
     * @lua NA
     */
    bool initWithArray(Array* pArrayOfItems);

    /** align items vertically */
    void alignItemsVertically();
    /** align items vertically with padding
    @since v0.7.2
    */
    void alignItemsVerticallyWithPadding(float padding);

    /** align items horizontally */
    void alignItemsHorizontally();
    /** align items horizontally with padding
    @since v0.7.2
    */
    void alignItemsHorizontallyWithPadding(float padding);

    /** align items in rows of columns 
     * @code
     * when this function bound to js,the input params are changed
     * js:var alignItemsInColumns(...)
     * @endcode
     * @lua NA
     */
    void alignItemsInColumns(unsigned int columns, ...);
    /**
     * @js NA
     * @lua NA
     */
    void alignItemsInColumns(unsigned int columns, va_list args);
    /**
     * @js NA
     */
    void alignItemsInColumnsWithArray(Array* rows);

    /** align items in columns of rows 
     * @code
     * when this function bound to js,the input params are changed
     * js:var alignItemsInRows(...)
     * @endcode
     * @lua NA
     */
    void alignItemsInRows(unsigned int rows, ...);
    /**
     * @js NA
     * @lua NA
     */
    void alignItemsInRows(unsigned int rows, va_list args);
    /**
     * @js NA
     */
    void alignItemsInRowsWithArray(Array* columns);

    //super methods
    virtual void addChild(Node * child);
    virtual void addChild(Node * child, int zOrder);
    virtual void addChild(Node * child, int zOrder, int tag);
    virtual void removeChild(Node* child, bool cleanup);

    /**
    @brief For phone event handle functions
    */
    virtual bool ccTouchBegan(Touch* touch);
    virtual void ccTouchEnded(Touch* touch);
    virtual void ccTouchCancelled(Touch *touch);
    virtual void ccTouchMoved(Touch* touch);

    /**
    @since v0.99.5
    override onExit
    *  @js NA
    *  @lua NA
    */
    virtual void onExit();

    virtual void setOpacityModifyRGB(bool bValue) {AX_UNUSED_PARAM(bValue);}
    virtual bool isOpacityModifyRGB(void) { return false;}
    
    virtual bool isEnabled() { return m_bEnabled; }
    virtual void setEnabled(bool value) { m_bEnabled = value; };

protected:
    MenuItem* itemForTouch(Touch * touch);
    tCCMenuState m_eState;
    MenuItem *m_pSelectedItem;
};

// end of GUI group
/// @}
/// @}

NS_AX_END

#endif//__AXMENU_H_
