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

#ifndef __AXLAYER_H__
#define __AXLAYER_H__

#include "base_nodes/Node.h"
#include "Protocols.h"
#include "platform/AccelerometerDelegate.h"
#include "keypad_dispatcher/KeypadDelegate.h"
#include "cocoa/Array.h"
#ifdef EMSCRIPTEN
#include "base_nodes/GLBufferedNode.h"
#endif // EMSCRIPTEN

NS_AX_BEGIN

typedef enum {
	kCCTouchesAllAtOnce,
	kCCTouchesOneByOne,
} ccTouchesMode;

/**
 * @addtogroup layer
 * @{
 */

class CCTouchScriptHandlerEntry;

//
// Layer
//
/** @brief Layer is a subclass of Node that implements the TouchEventsDelegate protocol.

All features from Node are valid, plus the following new features:
- It can receive iPhone Touches
- It can receive Accelerometer input
*/
class AX_DLL Layer : public Node, public AccelerometerDelegate, public KeypadDelegate
{
public:
    /**
     *  @js ctor
     */
    Layer();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~Layer();
    virtual bool init();
    
    /** create one layer */
    static Layer *create(void);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnter();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onExit();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void onEnterTransitionDidFinish();
    
    // default implements are used to call script callback if exist
    virtual bool ccTouchBegan(Touch *pTouch);
    virtual void ccTouchMoved(Touch *pTouch);
    virtual void ccTouchEnded(Touch *pTouch);

    virtual void keyDown(KeyCode keyCode);
    virtual void keyRepeat(KeyCode keyCode);
    virtual void keyUp(KeyCode keyCode);
    /**
     * @js NA
     * @lua NA
     */
    virtual void didAccelerate(Acceleration* pAccelerationValue);
    void registerScriptAccelerateHandler(int nHandler);
    void unregisterScriptAccelerateHandler(void);

    /** If isTouchEnabled, this method is called onEnter. Override it to change the
    way Layer receives touch events.
    ( Default: TouchDispatcher::sharedDispatcher()->addStandardDelegate(this,0); )
    Example:
    void Layer::registerWithTouchDispatcher()
    {
    TouchDispatcher::sharedDispatcher()->addTargetedDelegate(this,INT_MIN+1,true);
    }
    @since v0.8.0
    */
    virtual void registerWithTouchDispatcher(void);
    
    /** Register script touch events handler */
    virtual void registerScriptTouchHandler(int nHandler, bool bIsMultiTouches = false, int nPriority = INT_MIN, bool bSwallowsTouches = false);
    /** Unregister script touch events handler */
    virtual void unregisterScriptTouchHandler(void);

    /** whether or not it will receive Touch events.
    You can enable / disable touch events with this property.
    Only the touches of this node will be affected. This "method" is not propagated to it's children.
    @since v0.8.1
    */
    virtual bool isTouchEnabled();
    virtual void setTouchEnabled(bool value);
    
    /** priority of the touch events. Default is 0 */
    virtual void setTouchPriority(int priority);
    virtual int getTouchPriority();

    virtual bool isKeyboardEnabled();
    virtual void setKeyboardEnabled(bool value);

    virtual void setKeyboardPriority(int priority);
    virtual int getKeyboardPriority() const { return _keyboardPriority; }

    /** whether or not it will receive Accelerometer events
    You can enable / disable accelerometer events with this property.
    @since v0.8.1
    */
    virtual bool isAccelerometerEnabled();
    virtual void setAccelerometerEnabled(bool value);
    virtual void setAccelerometerInterval(double interval);

    /** whether or not it will receive keypad events
    You can enable / disable accelerometer events with this property.
    it's new in cocos2d-x
    */
    virtual bool isKeypadEnabled();
    virtual void setKeypadEnabled(bool value);

    /** Register keypad events handler */
    void registerScriptKeypadHandler(int nHandler);
    /** Unregister keypad events handler */
    void unregisterScriptKeypadHandler(void);

    virtual void keyBackClicked(void);
    virtual void keyMenuClicked(void);
    
    inline CCTouchScriptHandlerEntry* getScriptTouchHandlerEntry() { return m_pScriptTouchHandlerEntry; };
    inline CCScriptHandlerEntry* getScriptKeypadHandlerEntry() { return m_pScriptKeypadHandlerEntry; };
    inline CCScriptHandlerEntry* getScriptAccelerateHandlerEntry() { return m_pScriptAccelerateHandlerEntry; };
protected:   
    bool _touchEnabled;
    bool _keyboardEnabled;
    bool m_bAccelerometerEnabled;
    bool m_bKeypadEnabled;
    
private:
    // Script touch events handler
    CCTouchScriptHandlerEntry* m_pScriptTouchHandlerEntry;
    CCScriptHandlerEntry* m_pScriptKeypadHandlerEntry;
    CCScriptHandlerEntry* m_pScriptAccelerateHandlerEntry;
    
    int _touchPriority;
    TouchHandler* _touchHandler;

    int _keyboardPriority;
    KeyboardHandler* _keyboardHandler;
    
    int  excuteScriptTouchHandler(int nEventType, Touch *pTouch);
    int  excuteScriptTouchHandler(int nEventType, Set *pTouches);
};

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
#pragma mark -
#pragma mark LayerRGBA
#endif

/** LayerRGBA is a subclass of Layer that implements the RGBAProtocol protocol using a solid color as the background.
 
 All features from Layer are valid, plus the following new features that propagate into children that conform to the RGBAProtocol:
 - opacity
 - RGB colors
 @since 2.1
 */
class AX_DLL LayerRGBA : public Layer, public RGBAProtocol
{
public:
    CREATE_FUNC(LayerRGBA);
    /**
     *  @js ctor
     */
    LayerRGBA();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~LayerRGBA();
    
    virtual bool init();
    
    virtual GLubyte getOpacity();
    virtual GLubyte getDisplayedOpacity();
    virtual void setOpacity(GLubyte opacity);
    virtual void updateDisplayedOpacity(GLubyte parentOpacity);
    virtual bool isCascadeOpacityEnabled();
    virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
    
    virtual const ccColor3B& getColor();
    virtual const ccColor3B& getDisplayedColor();
    virtual void setColor(const ccColor3B& color);
    virtual void updateDisplayedColor(const ccColor3B& parentColor);
    virtual bool isCascadeColorEnabled();
    virtual void setCascadeColorEnabled(bool cascadeColorEnabled);
    
    virtual void setOpacityModifyRGB(bool bValue) {AX_UNUSED_PARAM(bValue);}
    virtual bool isOpacityModifyRGB() { return false; }
protected:
	GLubyte		_displayedOpacity, _realOpacity;
	ccColor3B	_displayedColor, _realColor;
	bool		_cascadeOpacityEnabled, _cascadeColorEnabled;
};

//
// LayerColor
//
/** @brief LayerColor is a subclass of Layer that implements the RGBAProtocol protocol.

All features from Layer are valid, plus the following new features:
- opacity
- RGB colors
*/
class AX_DLL LayerColor : public LayerRGBA, public BlendProtocol
#ifdef EMSCRIPTEN
, public CCGLBufferedNode
#endif // EMSCRIPTEN
{
protected:
    ccVertex2F m_pSquareVertices[4];
    ccColor4F  m_pSquareColors[4];

public:
    /**
     *  @js ctor
     */
    LayerColor();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual ~LayerColor();

    virtual void draw();
    virtual void setContentSize(const Size & var);
    
    static LayerColor* create();
    
    /** creates a Layer with color, width and height in Points */
    static LayerColor * create(const ccColor4B& color, GLfloat width, GLfloat height);
    /** creates a Layer with color. Width and height are the window size. */
    static LayerColor * create(const ccColor4B& color);

    virtual bool init();
    /** initializes a Layer with color, width and height in Points */
    virtual bool initWithColor(const ccColor4B& color, GLfloat width, GLfloat height);
    /** initializes a Layer with color. Width and height are the window size. */
    virtual bool initWithColor(const ccColor4B& color);

    /** change width in Points*/
    void changeWidth(GLfloat w);
    /** change height in Points*/
    void changeHeight(GLfloat h);
    /** change width and height in Points
    @since v0.8
    */
    void changeWidthAndHeight(GLfloat w ,GLfloat h);

    /** BlendFunction. Conforms to BlendProtocol protocol */
    AX_PROPERTY(ccBlendFunc, m_tBlendFunc, BlendFunc)
   
    virtual void setColor(const ccColor3B &color);
    virtual void setOpacity(GLubyte opacity);

protected:
    virtual void updateColor();
};

//
// LayerGradient
//
/** @brief LayerGradient is a subclass of LayerColor that draws gradients across the background.

All features from LayerColor are valid, plus the following new features:
- direction
- final color
- interpolation mode

Color is interpolated between the startColor and endColor along the given
vector (starting at the origin, ending at the terminus).  If no vector is
supplied, it defaults to (0, -1) -- a fade from top to bottom.

If 'compressedInterpolation' is disabled, you will not see either the start or end color for
non-cardinal vectors; a smooth gradient implying both end points will be still
be drawn, however.

If ' compressedInterpolation' is enabled (default mode) you will see both the start and end colors of the gradient.

@since v0.99.5
*/
class AX_DLL LayerGradient : public LayerColor
{
public:

    /** Creates a full-screen Layer with a gradient between start and end. */
    static LayerGradient* create(const ccColor4B& start, const ccColor4B& end);

    /** Creates a full-screen Layer with a gradient between start and end in the direction of v. */
    static LayerGradient* create(const ccColor4B& start, const ccColor4B& end, const Point& v);

    virtual bool init();
    /** Initializes the Layer with a gradient between start and end. 
     *  @js init
     */
    virtual bool initWithColor(const ccColor4B& start, const ccColor4B& end);

    /** Initializes the Layer with a gradient between start and end in the direction of v. 
     *  @js init
     */
    virtual bool initWithColor(const ccColor4B& start, const ccColor4B& end, const Point& v);

    AX_PROPERTY_PASS_BY_REF(ccColor3B, m_startColor, StartColor)
    AX_PROPERTY_PASS_BY_REF(ccColor3B, m_endColor, EndColor)
    AX_PROPERTY(GLubyte, m_cStartOpacity, StartOpacity)
    AX_PROPERTY(GLubyte, m_cEndOpacity, EndOpacity)
    AX_PROPERTY_PASS_BY_REF(Point, m_AlongVector, Vector)

    /** Whether or not the interpolation will be compressed in order to display all the colors of the gradient both in canonical and non canonical vectors
    Default: YES
    */
protected:
    bool m_bCompressedInterpolation;
public:
    virtual void setCompressedInterpolation(bool bCompressedInterpolation);
    virtual bool isCompressedInterpolation();
    
    static LayerGradient* create();

protected:
    virtual void updateColor();
};


/** @brief CCMultipleLayer is a Layer with the ability to multiplex it's children.
Features:
- It supports one or more children
- Only one children will be active a time
*/
class AX_DLL LayerMultiplex : public Layer
{
protected:
    unsigned int m_nEnabledLayer;
    Array*     m_pLayers;
public:
    /**
     * @js ctor
     * @lua NA
     */
    LayerMultiplex();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~LayerMultiplex();
    /**
     * @js NA
     */
    static LayerMultiplex* create();
    
    /** creates a CCMultiplexLayer with an array of layers.
     * @since v2.1
     * @js NA
     */
    static LayerMultiplex* createWithArray(Array* arrayOfLayers);

    /** creates a LayerMultiplex with one or more layers using a variable argument list. 
     * @lua NA
     */
    static LayerMultiplex * create(Layer* layer, ... );

    /**
     * lua script can not init with undetermined number of variables
     * so add these functions to be used with lua.
     */
    static LayerMultiplex * createWithLayer(Layer* layer);

    void addLayer(Layer* layer);

    /** initializes a MultiplexLayer with one or more layers using a variable argument list. 
     *  @js NA
     *  @lua NA
     */
    bool initWithLayers(Layer* layer, va_list params);
    /** switches to a certain layer indexed by n. 
    The current (old) layer will be removed from it's parent with 'cleanup:YES'.
    */

    /** initializes a CCMultiplexLayer with an array of layers
     *  @since v2.1
     *  @lua NA
     */
    bool initWithArray(Array* arrayOfLayers);

    void switchTo(unsigned int n);
    /** release the current layer and switches to another layer indexed by n.
    The current (old) layer will be removed from it's parent with 'cleanup:YES'.
    */
    void switchToAndReleaseMe(unsigned int n);
};


// end of layer group
/// @}

NS_AX_END

#endif // __AXLAYER_H__

