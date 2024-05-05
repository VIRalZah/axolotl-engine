/****************************************************************************
Copyright (c) 2024 zahann.ru

http://www.zahann.ru

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

#ifndef __AX_EVENT_H__
#define __AX_EVENT_H__

#include <vector>
#include <functional>

NS_AX_BEGIN

class AX_DLL Handler : public Object 
{
public:
	Handler();
	virtual ~Handler();

	int priority;
};

class AX_DLL Event
{
public:
	Event();
	virtual ~Event();
};

class Touch;

typedef std::function<bool(Touch*)> axTouchBeganCallback;
typedef std::function<void(Touch*)> axTouchCallback;

typedef std::vector<Touch*> TouchVector;

class AX_DLL TouchHandler : public Handler
{
public:
	axTouchBeganCallback onTouchBegan;
	axTouchCallback onTouchMoved;
	axTouchCallback onTouchEnded;
private:
	TouchVector _claimedTouches;

	friend class EventDispatcher;
};

enum TouchType
{
	BEGAN = 1,
	MOVED,
	ENDED
};

class AX_DLL EventTouch : public Event
{
public:
	EventTouch(Vec2 position, TouchType eventType, int id);

	const Vec2& getPosition() const { return _position; }
	const TouchType& getTouchType() const { return _eventType; }
	int getID() { return _id; }
protected:
	Vec2 _position;
	TouchType _eventType;
	int _id;
};

enum KeyboardEventType
{
	KEY_DOWN = 1,
	KEY_REPEAT,
	KEY_UP,
};

enum KeyCode
{
	KEY_NONE = 0,
	KEY_ESCAPE
};

typedef std::function<void(KeyCode)> axKeyCallback;

class AX_DLL KeyboardHandler : public Handler
{
public:
	axKeyCallback onKeyDown;
	axKeyCallback onKeyRepeat;
	axKeyCallback onKeyUp;
};

class AX_DLL EventKeyboard : public Event
{
public:
	EventKeyboard(KeyboardEventType eventType, KeyCode keyCode);

	const KeyboardEventType& getType() const { return _eventType; }
	const KeyCode& getKeyCode() const { return _keyCode; }
protected:
	KeyboardEventType _eventType;
	KeyCode _keyCode;
};

NS_AX_END

#endif // __AX_EVENT_H__