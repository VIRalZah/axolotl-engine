#include "GLView.h"

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

GLView::GLView()
: _delegate(NULL)
, _scaleX(1.0f)
, _scaleY(1.0f)
, _resolutionPolicy(kResolutionUnKnown)
{
}

GLView::~GLView()
{
}

void GLView::setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy)
{
    if (width == 0.0f || height == 0.0f)
    {
        return;
    }

    _designResolutionSize.setSize(width, height);

    _resolutionPolicy = resolutionPolicy;

    updateDesignResolutionSize();
}

const Size& GLView::getDesignResolutionSize() const 
{
    return _designResolutionSize;
}

const Size& GLView::getFrameSize() const
{
    return _screenSize;
}

void GLView::setFrameSize(float width, float height)
{
    _screenSize.setSize(width, height);

    if (_designResolutionSize.equals(Vec2::ZERO))
    {
        _designResolutionSize.setSize(width, height);
    }
}

Size  GLView::getVisibleSize() const
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

Vec2 GLView::getVisibleOrigin() const
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

void GLView::setTouchDelegate(EGLTouchDelegate * pDelegate)
{
    _delegate = pDelegate;
}

void GLView::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * _scaleX + _viewPortRect.origin.x),
               (GLint)(y * _scaleY + _viewPortRect.origin.y),
               (GLsizei)(w * _scaleX),
               (GLsizei)(h * _scaleY));
}

void GLView::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * _scaleX + _viewPortRect.origin.x),
              (GLint)(y * _scaleY + _viewPortRect.origin.y),
              (GLsizei)(w * _scaleX),
              (GLsizei)(h * _scaleY));
}

bool GLView::isScissorEnabled()
{
	return (GL_FALSE == glIsEnabled(GL_SCISSOR_TEST)) ? false : true;
}

Rect GLView::getScissorRect()
{
	GLfloat params[4];
	glGetFloatv(GL_SCISSOR_BOX, params);
	float x = (params[0] - _viewPortRect.origin.x) / _scaleX;
	float y = (params[1] - _viewPortRect.origin.y) / _scaleY;
	float w = params[2] / _scaleX;
	float h = params[3] / _scaleY;
	return Rect(x, y, w, h);
}

void GLView::setViewName(const std::string& viewName)
{
    _viewName = viewName;
}

std::string GLView::getViewName()
{
    return _viewName;
}

void GLView::handleTouchesBegin(int num, int ids[], float xs[], float ys[])
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

void GLView::handleTouchesMove(int num, int ids[], float xs[], float ys[])
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

void GLView::handleTouchesEnd(int num, int ids[], float xs[], float ys[])
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

void GLView::updateDesignResolutionSize()
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
        director->createDebugInfoLabel();
        director->setGLDefaultValues();
    }
}

const Rect& GLView::getViewPortRect() const
{
    return _viewPortRect;
}

float GLView::getScaleX() const
{
    return _scaleX;
}

float GLView::getScaleY() const
{
    return _scaleY;
}

NS_AX_END
