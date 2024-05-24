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

#include <stdarg.h>
#include "Layer.h"

#include "keypad_dispatcher/KeypadDispatcher.h"
#include "Accelerometer.h"
#include "base/Director.h"
#include "support/PointExtension.h"
#include "shaders/ShaderCache.h"
#include "shaders/GLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "support/TransformUtils.h"
// extern
#include "kazmath/GL/matrix.h"
#include "base/EventDispatcher.h"

NS_AX_BEGIN

// Layer
Layer::Layer()
: _touchEnabled(false)
, _keyboardEnabled(false)
, m_bAccelerometerEnabled(false)
, m_bKeypadEnabled(false)
, _touchPriority(0)
, _touchHandler(nullptr)
, _keyboardPriority(0)
, _keyboardHandler(nullptr)
{
    m_bIgnoreAnchorPointForPosition = true;
    setAnchorPoint(Vec2(0.5f, 0.5f));
}

Layer::~Layer()
{
}

bool Layer::init()
{
    bool bRet = false;
    do 
    {        
        Director * pDirector;
        AX_BREAK_IF(!(pDirector = Director::sharedDirector()));
        this->setContentSize(pDirector->getWinSize());
        _touchEnabled = false;
        m_bAccelerometerEnabled = false;
        // success
        bRet = true;
    } while(0);
    return bRet;
}

Layer *Layer::create()
{
    Layer *pRet = new Layer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        AX_SAFE_DELETE(pRet);
        return NULL;
    }
}

/// Touch and Accelerometer related

void Layer::registerWithTouchDispatcher()
{
    if (!_touchHandler)
    {
        _touchHandler = new TouchHandler();

        _touchHandler->onTouchBegan = std::bind(&Layer::ccTouchBegan, this, std::placeholders::_1);
        _touchHandler->onTouchMoved = std::bind(&Layer::ccTouchMoved, this, std::placeholders::_1);
        _touchHandler->onTouchEnded = std::bind(&Layer::ccTouchEnded, this, std::placeholders::_1);

        _touchHandler->priority = _touchPriority;
    }

    Director::sharedDirector()->getEventDispatcher()->addHandler(_touchHandler);
}

/// isTouchEnabled getter
bool Layer::isTouchEnabled()
{
    return _touchEnabled;
}
/// isTouchEnabled setter
void Layer::setTouchEnabled(bool enabled)
{
    if (_touchEnabled != enabled)
    {
        _touchEnabled = enabled;
        if (m_bRunning)
        {
            if (enabled)
            {
                registerWithTouchDispatcher();
            }
            else
            {
                Director::sharedDirector()->getEventDispatcher()->removeHandler(_touchHandler);
                AX_SAFE_RELEASE_NULL(_touchHandler);
            }
        }
    }
}

void Layer::setTouchPriority(int priority)
{
    if (_touchPriority != priority)
    {
        _touchPriority = priority;
        
		if( _touchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

int Layer::getTouchPriority()
{
    return _touchPriority;
}

bool Layer::isKeyboardEnabled()
{
    return _keyboardEnabled;
}

void Layer::setKeyboardEnabled(bool enabled)
{
    if (_keyboardEnabled != enabled)
    {
        _keyboardEnabled = enabled;
        if (m_bRunning)
        {
            if (enabled)
            {
                if (!_keyboardHandler)
                {
                    _keyboardHandler = new KeyboardHandler();

                    _keyboardHandler->onKeyDown = std::bind(&Layer::keyDown, this, std::placeholders::_1);
                    _keyboardHandler->onKeyUp = std::bind(&Layer::keyUp, this, std::placeholders::_1);

                    _keyboardHandler->priority = _keyboardPriority;
                }
            }
            else
            {
                Director::sharedDirector()->getEventDispatcher()->removeHandler(_keyboardHandler);
                AX_SAFE_RELEASE_NULL(_keyboardHandler);
            }
        }
    }
}

void Layer::setKeyboardPriority(int priority)
{
    if (_keyboardPriority != priority)
    {
        _keyboardPriority = priority;

        if (_keyboardEnabled)
        {
            setKeyboardEnabled(false);
            setKeyboardEnabled(true);
        }
    }
}

/// isAccelerometerEnabled getter
bool Layer::isAccelerometerEnabled()
{
    return m_bAccelerometerEnabled;
}
/// isAccelerometerEnabled setter
void Layer::setAccelerometerEnabled(bool enabled)
{
    if (enabled != m_bAccelerometerEnabled)
    {
        m_bAccelerometerEnabled = enabled;

        if (m_bRunning)
        {
            Director* pDirector = Director::sharedDirector();
            if (enabled)
            {
                pDirector->getAccelerometer()->setDelegate(this);
            }
            else
            {
                pDirector->getAccelerometer()->setDelegate(NULL);
            }
        }
    }
}


void Layer::setAccelerometerInterval(double interval) {
    if (m_bAccelerometerEnabled)
    {
        if (m_bRunning)
        {
            Director* pDirector = Director::sharedDirector();
            pDirector->getAccelerometer()->setAccelerometerInterval(interval);
        }
    }
}


void Layer::didAccelerate(Acceleration* pAccelerationValue)
{
   AX_UNUSED_PARAM(pAccelerationValue);
}

/// isKeypadEnabled getter
bool Layer::isKeypadEnabled()
{
    return m_bKeypadEnabled;
}
/// isKeypadEnabled setter
void Layer::setKeypadEnabled(bool enabled)
{
    if (enabled != m_bKeypadEnabled)
    {
        m_bKeypadEnabled = enabled;

        if (m_bRunning)
        {
            Director* pDirector = Director::sharedDirector();
            if (enabled)
            {
                pDirector->getKeypadDispatcher()->addDelegate(this);
            }
            else
            {
                pDirector->getKeypadDispatcher()->removeDelegate(this);
            }
        }
    }
}

void Layer::keyBackClicked()
{
}

void Layer::keyMenuClicked()
{
}

/// Callbacks
void Layer::onEnter()
{
    Director* pDirector = Director::sharedDirector();

    if (_touchEnabled)
    {
        registerWithTouchDispatcher();
    }

    if (_keyboardEnabled)
    {
        pDirector->getEventDispatcher()->addHandler(_keyboardHandler);
    }

    // then iterate over all the children
    Node::onEnter();

    // add this layer to concern the Accelerometer Sensor
    if (m_bAccelerometerEnabled)
    {
        pDirector->getAccelerometer()->setDelegate(this);
    }

    // add this layer to concern the keypad msg
    if (m_bKeypadEnabled)
    {
        pDirector->getKeypadDispatcher()->addDelegate(this);
    }
}

void Layer::onExit()
{
    auto eventDispatcher = Director::sharedDirector()->getEventDispatcher();
    if(_touchEnabled)
    {
        eventDispatcher->removeHandler(_touchHandler);
    }

    if (_keyboardEnabled)
    {
        eventDispatcher->removeHandler(_keyboardHandler);
    }

    if (m_bAccelerometerEnabled)
    {
        Director::sharedDirector()->getAccelerometer()->setDelegate(NULL);
    }

    if (m_bKeypadEnabled)
    {
        Director::sharedDirector()->getKeypadDispatcher()->removeDelegate(this);
    }

    Node::onExit();
}

void Layer::onEnterTransitionDidFinish()
{
    if (m_bAccelerometerEnabled)
    {
        Director* pDirector = Director::sharedDirector();
        pDirector->getAccelerometer()->setDelegate(this);
    }
    
    Node::onEnterTransitionDidFinish();
}

bool Layer::ccTouchBegan(Touch *pTouch)
{
    AX_UNUSED_PARAM(pTouch);
   
    AXAssert(false, "Layer#ccTouchBegan override me");
    return true;
}

void Layer::ccTouchMoved(Touch *pTouch)
{
    AX_UNUSED_PARAM(pTouch);
}
    
void Layer::ccTouchEnded(Touch *pTouch)
{
    AX_UNUSED_PARAM(pTouch);
}

void Layer::keyDown(KeyCode keyCode)
{
}

void Layer::keyUp(KeyCode keyCode)
{
}

// LayerRGBA
LayerRGBA::LayerRGBA()
: _displayedOpacity(255)
, _realOpacity (255)
, _displayedColor(ccWHITE)
, _realColor(ccWHITE)
, _cascadeOpacityEnabled(false)
, _cascadeColorEnabled(false)
{}

LayerRGBA::~LayerRGBA() {}

bool LayerRGBA::init()
{
	if (Layer::init())
    {
        _displayedOpacity = _realOpacity = 255;
        _displayedColor = _realColor = ccWHITE;
        setCascadeOpacityEnabled(false);
        setCascadeColorEnabled(false);
        
        return true;
    }
    else
    {
        return false;
    }
}

GLubyte LayerRGBA::getOpacity()
{
	return _realOpacity;
}

GLubyte LayerRGBA::getDisplayedOpacity()
{
	return _displayedOpacity;
}

/** Override synthesized setOpacity to recurse items */
void LayerRGBA::setOpacity(GLubyte opacity)
{
	_displayedOpacity = _realOpacity = opacity;
    
	if( _cascadeOpacityEnabled )
    {
		GLubyte parentOpacity = 255;
        RGBAProtocol *parent = dynamic_cast<RGBAProtocol*>(m_pParent);
        if (parent && parent->isCascadeOpacityEnabled())
        {
            parentOpacity = parent->getDisplayedOpacity();
        }
        updateDisplayedOpacity(parentOpacity);
	}
}

const ccColor3B& LayerRGBA::getColor()
{
	return _realColor;
}

const ccColor3B& LayerRGBA::getDisplayedColor()
{
	return _displayedColor;
}

void LayerRGBA::setColor(const ccColor3B& color)
{
	_displayedColor = _realColor = color;
	
	if (_cascadeColorEnabled)
    {
		ccColor3B parentColor = ccWHITE;
        RGBAProtocol* parent = dynamic_cast<RGBAProtocol*>(m_pParent);
		if (parent && parent->isCascadeColorEnabled())
        {
            parentColor = parent->getDisplayedColor();
        }

        updateDisplayedColor(parentColor);
	}
}

void LayerRGBA::updateDisplayedOpacity(GLubyte parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity/255.0;
    
    if (_cascadeOpacityEnabled)
    {
        Object *obj = NULL;
        AXARRAY_FOREACH(m_pChildren, obj)
        {
            RGBAProtocol *item = dynamic_cast<RGBAProtocol*>(obj);
            if (item)
            {
                item->updateDisplayedOpacity(_displayedOpacity);
            }
        }
    }
}

void LayerRGBA::updateDisplayedColor(const ccColor3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r/255.0;
	_displayedColor.g = _realColor.g * parentColor.g/255.0;
	_displayedColor.b = _realColor.b * parentColor.b/255.0;
    
    if (_cascadeColorEnabled)
    {
        Object *obj = NULL;
        AXARRAY_FOREACH(m_pChildren, obj)
        {
            RGBAProtocol *item = dynamic_cast<RGBAProtocol*>(obj);
            if (item)
            {
                item->updateDisplayedColor(_displayedColor);
            }
        }
    }
}

bool LayerRGBA::isCascadeOpacityEnabled()
{
    return _cascadeOpacityEnabled;
}

void LayerRGBA::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
    _cascadeOpacityEnabled = cascadeOpacityEnabled;
}

bool LayerRGBA::isCascadeColorEnabled()
{
    return _cascadeColorEnabled;
}

void LayerRGBA::setCascadeColorEnabled(bool cascadeColorEnabled)
{
    _cascadeColorEnabled = cascadeColorEnabled;
}

/// LayerColor

LayerColor::LayerColor()
{
    // default blend function
    m_tBlendFunc.src = AX_BLEND_SRC;
    m_tBlendFunc.dst = AX_BLEND_DST;
}
    
LayerColor::~LayerColor()
{
}

/// blendFunc getter
ccBlendFunc LayerColor::getBlendFunc()
{
    return m_tBlendFunc;
}
/// blendFunc setter
void LayerColor::setBlendFunc(ccBlendFunc var)
{
    m_tBlendFunc = var;
}

LayerColor* LayerColor::create()
{
    LayerColor* pRet = new LayerColor();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

LayerColor * LayerColor::create(const ccColor4B& color, GLfloat width, GLfloat height)
{
    LayerColor * pLayer = new LayerColor();
    if( pLayer && pLayer->initWithColor(color,width,height))
    {
        pLayer->autorelease();
        return pLayer;
    }
    AX_SAFE_DELETE(pLayer);
    return NULL;
}

LayerColor * LayerColor::create(const ccColor4B& color)
{
    LayerColor * pLayer = new LayerColor();
    if(pLayer && pLayer->initWithColor(color))
    {
        pLayer->autorelease();
        return pLayer;
    }
    AX_SAFE_DELETE(pLayer);
    return NULL;
}

bool LayerColor::init()
{
    Size s = Director::sharedDirector()->getWinSize();
    return initWithColor(ccc4(0,0,0,0), s.width, s.height);
}

bool LayerColor::initWithColor(const ccColor4B& color, GLfloat w, GLfloat h)
{
    if (Layer::init())
    {

        // default blend function
        m_tBlendFunc.src = GL_SRC_ALPHA;
        m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

        _displayedColor.r = _realColor.r = color.r;
        _displayedColor.g = _realColor.g = color.g;
        _displayedColor.b = _realColor.b = color.b;
        _displayedOpacity = _realOpacity = color.a;

        for (size_t i = 0; i<sizeof(m_pSquareVertices) / sizeof( m_pSquareVertices[0]); i++ )
        {
            m_pSquareVertices[i].x = 0.0f;
            m_pSquareVertices[i].y = 0.0f;
        }

        updateColor();
        setContentSize(Size(w, h));

        setShaderProgram(ShaderCache::sharedShaderCache()->programForKey(kAXShader_PositionColor));
    }
    return true;
}

bool LayerColor::initWithColor(const ccColor4B& color)
{
    Size s = Director::sharedDirector()->getWinSize();
    this->initWithColor(color, s.width, s.height);
    return true;
}

/// override contentSize
void LayerColor::setContentSize(const Size & size)
{
    m_pSquareVertices[1].x = size.width;
    m_pSquareVertices[2].y = size.height;
    m_pSquareVertices[3].x = size.width;
    m_pSquareVertices[3].y = size.height;

    Layer::setContentSize(size);
}

void LayerColor::changeWidthAndHeight(GLfloat w ,GLfloat h)
{
    this->setContentSize(Size(w, h));
}

void LayerColor::changeWidth(GLfloat w)
{
    this->setContentSize(Size(w, m_obContentSize.height));
}

void LayerColor::changeHeight(GLfloat h)
{
    this->setContentSize(Size(m_obContentSize.width, h));
}

void LayerColor::updateColor()
{
    for( unsigned int i=0; i < 4; i++ )
    {
        m_pSquareColors[i].r = _displayedColor.r / 255.0f;
        m_pSquareColors[i].g = _displayedColor.g / 255.0f;
        m_pSquareColors[i].b = _displayedColor.b / 255.0f;
        m_pSquareColors[i].a = _displayedOpacity / 255.0f;
    }
}

void LayerColor::draw()
{
    AX_NODE_DRAW_SETUP();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color );

    //
    // Attributes
    //
#ifdef EMSCRIPTEN
    setGLBufferData(m_pSquareVertices, 4 * sizeof(ccVertex2F), 0);
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLBufferData(m_pSquareColors, 4 * sizeof(ccColor4F), 1);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, m_pSquareVertices);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, m_pSquareColors);
#endif // EMSCRIPTEN

    ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    AX_INCREMENT_GL_DRAWS(1);
}

void LayerColor::setColor(const ccColor3B &color)
{
    LayerRGBA::setColor(color);
    updateColor();
}

void LayerColor::setOpacity(GLubyte opacity)
{
    LayerRGBA::setOpacity(opacity);
    updateColor();
}

//
// LayerGradient
// 

LayerGradient* LayerGradient::create(const ccColor4B& start, const ccColor4B& end)
{
    LayerGradient * pLayer = new LayerGradient();
    if( pLayer && pLayer->initWithColor(start, end))
    {
        pLayer->autorelease();
        return pLayer;
    }
    AX_SAFE_DELETE(pLayer);
    return NULL;
}

LayerGradient* LayerGradient::create(const ccColor4B& start, const ccColor4B& end, const Vec2& v)
{
    LayerGradient * pLayer = new LayerGradient();
    if( pLayer && pLayer->initWithColor(start, end, v))
    {
        pLayer->autorelease();
        return pLayer;
    }
    AX_SAFE_DELETE(pLayer);
    return NULL;
}

LayerGradient* LayerGradient::create()
{
    LayerGradient* pRet = new LayerGradient();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool LayerGradient::init()
{
	return initWithColor(ccc4(0, 0, 0, 255), ccc4(0, 0, 0, 255));
}

bool LayerGradient::initWithColor(const ccColor4B& start, const ccColor4B& end)
{
    return initWithColor(start, end, Vec2(0, -1));
}

bool LayerGradient::initWithColor(const ccColor4B& start, const ccColor4B& end, const Vec2& v)
{
    m_endColor.r  = end.r;
    m_endColor.g  = end.g;
    m_endColor.b  = end.b;

    m_cEndOpacity   = end.a;
    m_cStartOpacity    = start.a;
    m_AlongVector   = v;

    m_bCompressedInterpolation = true;

    return LayerColor::initWithColor(ccc4(start.r, start.g, start.b, 255));
}

void LayerGradient::updateColor()
{
    LayerColor::updateColor();

    float h = PointLength(m_AlongVector);
    if (h == 0)
        return;

    float c = sqrtf(2.0f);
    Vec2 u = Vec2(m_AlongVector.x / h, m_AlongVector.y / h);

    // Compressed Interpolation mode
    if (m_bCompressedInterpolation)
    {
        float h2 = 1 / ( fabsf(u.x) + fabsf(u.y) );
        u = PointMult(u, h2 * (float)c);
    }

    float opacityf = (float)_displayedOpacity / 255.0f;

    ccColor4F S = {
        _displayedColor.r / 255.0f,
        _displayedColor.g / 255.0f,
        _displayedColor.b / 255.0f,
        m_cStartOpacity * opacityf / 255.0f
    };

    ccColor4F E = {
        m_endColor.r / 255.0f,
        m_endColor.g / 255.0f,
        m_endColor.b / 255.0f,
        m_cEndOpacity * opacityf / 255.0f
    };

    // (-1, -1)
    m_pSquareColors[0].r = E.r + (S.r - E.r) * ((c + u.x + u.y) / (2.0f * c));
    m_pSquareColors[0].g = E.g + (S.g - E.g) * ((c + u.x + u.y) / (2.0f * c));
    m_pSquareColors[0].b = E.b + (S.b - E.b) * ((c + u.x + u.y) / (2.0f * c));
    m_pSquareColors[0].a = E.a + (S.a - E.a) * ((c + u.x + u.y) / (2.0f * c));
    // (1, -1)
    m_pSquareColors[1].r = E.r + (S.r - E.r) * ((c - u.x + u.y) / (2.0f * c));
    m_pSquareColors[1].g = E.g + (S.g - E.g) * ((c - u.x + u.y) / (2.0f * c));
    m_pSquareColors[1].b = E.b + (S.b - E.b) * ((c - u.x + u.y) / (2.0f * c));
    m_pSquareColors[1].a = E.a + (S.a - E.a) * ((c - u.x + u.y) / (2.0f * c));
    // (-1, 1)
    m_pSquareColors[2].r = E.r + (S.r - E.r) * ((c + u.x - u.y) / (2.0f * c));
    m_pSquareColors[2].g = E.g + (S.g - E.g) * ((c + u.x - u.y) / (2.0f * c));
    m_pSquareColors[2].b = E.b + (S.b - E.b) * ((c + u.x - u.y) / (2.0f * c));
    m_pSquareColors[2].a = E.a + (S.a - E.a) * ((c + u.x - u.y) / (2.0f * c));
    // (1, 1)
    m_pSquareColors[3].r = E.r + (S.r - E.r) * ((c - u.x - u.y) / (2.0f * c));
    m_pSquareColors[3].g = E.g + (S.g - E.g) * ((c - u.x - u.y) / (2.0f * c));
    m_pSquareColors[3].b = E.b + (S.b - E.b) * ((c - u.x - u.y) / (2.0f * c));
    m_pSquareColors[3].a = E.a + (S.a - E.a) * ((c - u.x - u.y) / (2.0f * c));
}

const ccColor3B& LayerGradient::getStartColor()
{
    return _realColor;
}

void LayerGradient::setStartColor(const ccColor3B& color)
{
    setColor(color);
}

void LayerGradient::setEndColor(const ccColor3B& color)
{
    m_endColor = color;
    updateColor();
}

const ccColor3B& LayerGradient::getEndColor()
{
    return m_endColor;
}

void LayerGradient::setStartOpacity(GLubyte o)
{
    m_cStartOpacity = o;
    updateColor();
}

GLubyte LayerGradient::getStartOpacity()
{
    return m_cStartOpacity;
}

void LayerGradient::setEndOpacity(GLubyte o)
{
    m_cEndOpacity = o;
    updateColor();
}

GLubyte LayerGradient::getEndOpacity()
{
    return m_cEndOpacity;
}

void LayerGradient::setVector(const Vec2& var)
{
    m_AlongVector = var;
    updateColor();
}

const Vec2& LayerGradient::getVector()
{
    return m_AlongVector;
}

bool LayerGradient::isCompressedInterpolation()
{
    return m_bCompressedInterpolation;
}

void LayerGradient::setCompressedInterpolation(bool compress)
{
    m_bCompressedInterpolation = compress;
    updateColor();
}

/// MultiplexLayer

LayerMultiplex::LayerMultiplex()
: m_nEnabledLayer(0)
, m_pLayers(NULL)
{
}
LayerMultiplex::~LayerMultiplex()
{
    AX_SAFE_RELEASE(m_pLayers);
}

LayerMultiplex * LayerMultiplex::create(Layer * layer, ...)
{
    va_list args;
    va_start(args,layer);

    LayerMultiplex * pMultiplexLayer = new LayerMultiplex();
    if(pMultiplexLayer && pMultiplexLayer->initWithLayers(layer, args))
    {
        pMultiplexLayer->autorelease();
        va_end(args);
        return pMultiplexLayer;
    }
    va_end(args);
    AX_SAFE_DELETE(pMultiplexLayer);
    return NULL;
}

LayerMultiplex * LayerMultiplex::createWithLayer(Layer* layer)
{
    return LayerMultiplex::create(layer, NULL);
}

LayerMultiplex* LayerMultiplex::create()
{
    LayerMultiplex* pRet = new LayerMultiplex();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

LayerMultiplex* LayerMultiplex::createWithArray(Array* arrayOfLayers)
{
    LayerMultiplex* pRet = new LayerMultiplex();
    if (pRet && pRet->initWithArray(arrayOfLayers))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

void LayerMultiplex::addLayer(Layer* layer)
{
    AXAssert(m_pLayers, "");
    m_pLayers->addObject(layer);
}

bool LayerMultiplex::initWithLayers(Layer *layer, va_list params)
{
    if (Layer::init())
    {
        m_pLayers = Array::createWithCapacity(5);
        m_pLayers->retain();
        m_pLayers->addObject(layer);

        Layer *l = va_arg(params,Layer*);
        while( l ) {
            m_pLayers->addObject(l);
            l = va_arg(params,Layer*);
        }

        m_nEnabledLayer = 0;
        this->addChild((Node*)m_pLayers->objectAtIndex(m_nEnabledLayer));
        return true;
    }

    return false;
}

bool LayerMultiplex::initWithArray(Array* arrayOfLayers)
{
    if (Layer::init())
    {
        m_pLayers = Array::createWithCapacity(arrayOfLayers->count());
        m_pLayers->addObjectsFromArray(arrayOfLayers);
        m_pLayers->retain();

        m_nEnabledLayer = 0;
        this->addChild((Node*)m_pLayers->objectAtIndex(m_nEnabledLayer));
        return true;
    }
    return false;
}

void LayerMultiplex::switchTo(unsigned int n)
{
    AXAssert( n < m_pLayers->count(), "Invalid index in MultiplexLayer switchTo message" );

    this->removeChild((Node*)m_pLayers->objectAtIndex(m_nEnabledLayer), true);

    m_nEnabledLayer = n;

    this->addChild((Node*)m_pLayers->objectAtIndex(n));
}

void LayerMultiplex::switchToAndReleaseMe(unsigned int n)
{
    AXAssert( n < m_pLayers->count(), "Invalid index in MultiplexLayer switchTo message" );

    this->removeChild((Node*)m_pLayers->objectAtIndex(m_nEnabledLayer), true);

    //[layers replaceObjectAtIndex:enabledLayer withObject:[NSNull null]];
    m_pLayers->replaceObjectAtIndex(m_nEnabledLayer, NULL);

    m_nEnabledLayer = n;

    this->addChild((Node*)m_pLayers->objectAtIndex(n));
}

NS_AX_END
