#include "EGLViewProtocol.h"

#include "base/Touch.h"
#include "base/Director.h"
#include "base/Set.h"
#include "base/Dictionary.h"
#include "base/Integer.h"
#include "base/EventDispatcher.h"

NS_AX_BEGIN

static Touch* s_pTouches[AX_MAX_TOUCHES] = { NULL };
static unsigned int s_indexBitsUsed = 0;
static Dictionary s_TouchesIntergerDict;

static int getUnUsedIndex()
{
    int i;
    int temp = s_indexBitsUsed;

    for (i = 0; i < AX_MAX_TOUCHES; i++) {
        if (! (temp & 0x00000001)) {
            s_indexBitsUsed |= (1 <<  i);
            return i;
        }

        temp >>= 1;
    }

    // all bits are used
    return -1;
}

static void removeUsedIndexBit(int index)
{
    if (index < 0 || index >= AX_MAX_TOUCHES) 
    {
        return;
    }

    unsigned int temp = 1 << index;
    temp = ~temp;
    s_indexBitsUsed &= temp;
}

EGLViewProtocol::EGLViewProtocol()
: _delegate(NULL)
, _scaleX(1.0f)
, _scaleY(1.0f)
, _resolutionPolicy(kResolutionUnKnown)
{
}

EGLViewProtocol::~EGLViewProtocol()
{

}

void EGLViewProtocol::setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy)
{
    if (width == 0.0f || height == 0.0f)
    {
        return;
    }

    _designResolutionSize.setSize(width, height);

    _resolutionPolicy = resolutionPolicy;

    updateDesignResolutionSize();
}

const Size& EGLViewProtocol::getDesignResolutionSize() const 
{
    return _designResolutionSize;
}

const Size& EGLViewProtocol::getFrameSize() const
{
    return _screenSize;
}

void EGLViewProtocol::setFrameSize(float width, float height)
{
    _screenSize.setSize(width, height);

    if (_designResolutionSize.equals(Vec2::ZERO))
    {
        _designResolutionSize.setSize(width, height);
    }
}

Size  EGLViewProtocol::getVisibleSize() const
{
    if (_resolutionPolicy == kResolutionNoBorder)
    {
        return Size(_screenSize.width/_scaleX, _screenSize.height/_scaleY);
    }
    else 
    {
        return _designResolutionSize;
    }
}

Vec2 EGLViewProtocol::getVisibleOrigin() const
{
    if (_resolutionPolicy == kResolutionNoBorder)
    {
        return Vec2((_designResolutionSize.width - _screenSize.width/_scaleX)/2, 
                           (_designResolutionSize.height - _screenSize.height/_scaleY)/2);
    }
    else 
    {
        return Vec2::ZERO;
    }
}

void EGLViewProtocol::setTouchDelegate(EGLTouchDelegate * pDelegate)
{
    _delegate = pDelegate;
}

void EGLViewProtocol::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * _scaleX + _viewPortRect.origin.x),
               (GLint)(y * _scaleY + _viewPortRect.origin.y),
               (GLsizei)(w * _scaleX),
               (GLsizei)(h * _scaleY));
}

void EGLViewProtocol::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * _scaleX + _viewPortRect.origin.x),
              (GLint)(y * _scaleY + _viewPortRect.origin.y),
              (GLsizei)(w * _scaleX),
              (GLsizei)(h * _scaleY));
}

bool EGLViewProtocol::isScissorEnabled()
{
	return (GL_FALSE == glIsEnabled(GL_SCISSOR_TEST)) ? false : true;
}

Rect EGLViewProtocol::getScissorRect()
{
	GLfloat params[4];
	glGetFloatv(GL_SCISSOR_BOX, params);
	float x = (params[0] - _viewPortRect.origin.x) / _scaleX;
	float y = (params[1] - _viewPortRect.origin.y) / _scaleY;
	float w = params[2] / _scaleX;
	float h = params[3] / _scaleY;
	return Rect(x, y, w, h);
}

void EGLViewProtocol::setViewName(const std::string& viewName)
{
    _viewName = viewName;
}

std::string EGLViewProtocol::getViewName()
{
    return _viewName;
}

void EGLViewProtocol::handleTouchesBegin(int num, int ids[], float xs[], float ys[])
{
    for (int i = 0; i < num; ++i)
    {
        int id = ids[i];
        float x = xs[i];
        float y = ys[i];

        EventTouch event(
            Vec2(
                (x - _viewPortRect.origin.x) / _scaleX,
                (y - _viewPortRect.origin.y) / _scaleY
            ),
            TouchType::BEGAN,
            id
        );
        Director::sharedDirector()->getEventDispatcher()->dispatchEvent(&event);
    }
}

void EGLViewProtocol::handleTouchesMove(int num, int ids[], float xs[], float ys[])
{
    for (int i = 0; i < num; ++i)
    {
        int id = ids[i];
        float x = xs[i];
        float y = ys[i];

        EventTouch event(
            Vec2(
                (x - _viewPortRect.origin.x) / _scaleX,
                (y - _viewPortRect.origin.y) / _scaleY
            ),
            TouchType::MOVED,
            id
            );
        Director::sharedDirector()->getEventDispatcher()->dispatchEvent(&event);
    }
}

void EGLViewProtocol::handleTouchesEnd(int num, int ids[], float xs[], float ys[])
{
    for (int i = 0; i < num; ++i)
    {
        int id = ids[i];
        float x = xs[i];
        float y = ys[i];

        EventTouch event(
            Vec2(
                (x - _viewPortRect.origin.x) / _scaleX,
                (y - _viewPortRect.origin.y) / _scaleY
            ),
            TouchType::ENDED,
            id
            );

        Director::sharedDirector()->getEventDispatcher()->dispatchEvent(&event);
    }
}

void EGLViewProtocol::updateDesignResolutionSize()
{
    _scaleX = (float)_screenSize.width / _designResolutionSize.width;
    _scaleY = (float)_screenSize.height / _designResolutionSize.height;

    if (_resolutionPolicy == kResolutionNoBorder)
    {
        _scaleX = _scaleY = MAX(_scaleX, _scaleY);
    }

    if (_resolutionPolicy == kResolutionShowAll)
    {
        _scaleX = _scaleY = MIN(_scaleX, _scaleY);
    }

    if (_resolutionPolicy == kResolutionFixedHeight)
    {
        _scaleX = _scaleY;
        _designResolutionSize.width = ceilf(_screenSize.width / _scaleX);
    }

    if (_resolutionPolicy == kResolutionFixedWidth)
    {
        _scaleY = _scaleX;
        _designResolutionSize.height = ceilf(_screenSize.height / _scaleY);
    }

    float viewPortW = _designResolutionSize.width * _scaleX;
    float viewPortH = _designResolutionSize.height * _scaleY;

    _viewPortRect.setRect((_screenSize.width - viewPortW) / 2, (_screenSize.height - viewPortH) / 2, viewPortW, viewPortH);

    auto director = Director::sharedDirector();
    if (director->getOpenGLView() == this)
    {
        director->_winSizeInPoints = _designResolutionSize;
        director->createStatsLabel();
        director->setGLDefaultValues();
    }
}

const Rect& EGLViewProtocol::getViewPortRect() const
{
    return _viewPortRect;
}

float EGLViewProtocol::getScaleX() const
{
    return _scaleX;
}

float EGLViewProtocol::getScaleY() const
{
    return _scaleY;
}

NS_AX_END
