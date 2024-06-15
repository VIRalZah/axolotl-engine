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

#include "Types.h"
#include "Object.h"
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
	const TouchType& getTouchType() const { return _type; }
	int getID() const { return _id; }
protected:
	Vec2 _position;
	TouchType _type;
	int _id;
};

enum KeyboardEventType
{
	KEY_UP = 0,
	KEY_DOWN,
};

enum KeyCode
{
	NONE = 0,
	ESCAPE,
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
	A,
	S,
	D
};

typedef std::function<void(KeyCode)> axKeyCallback;

class AX_DLL KeyboardHandler : public Handler
{
public:
	axKeyCallback onKeyDown;
	axKeyCallback onKeyUp;
};

class AX_DLL EventKeyboard : public Event
{
public:
	EventKeyboard(KeyboardEventType eventType, KeyCode keyCode);

	const KeyboardEventType& getType() const { return _type; }
	const KeyCode& getKeyCode() const { return _keyCode; }
protected:
	KeyboardEventType _type;
	KeyCode _keyCode;
};

class AX_DLL KeypadHandler : public Handler
{
public:
	axCallback onKeyBackClicked;
	axCallback onKeyMenuClicked;
};

enum KeypadEventType
{
	KEY_BACK_CLICKED = 0,
	KEY_MENU_CLICKED,
};

class AX_DLL EventKeypad : public Event
{
public:
	EventKeypad(KeypadEventType eventType);

	const KeypadEventType& getType() const { return _type; }
protected:
	KeypadEventType _type;
};

typedef std::function<void(const std::string& string)> axInputTextCallback;

class AX_DLL IMEHandler : public Handler
{
public:
	axInputTextCallback onInputText;
	axCallback onDeleteBackward;
};

enum IMEEventType
{
	INPUT_TEXT = 0,
	DELETE_BACKWARD
};

class AX_DLL EventIME : public Event
{
public:
	EventIME(const IMEEventType& type);
	EventIME(const IMEEventType& type, const wchar_t* text);
	EventIME(const IMEEventType& type, const char* text);

	const std::string& getString() const { return _utf8Text; }
	const IMEEventType& getType() const { return _type; }
protected:
	std::string _utf8Text;
	IMEEventType _type;
};

class AX_DLL CustomHandler : public Handler
{
public:
	std::string target;
	axCallback callback;
};

class AX_DLL EventCustom : public Event
{
public:
	EventCustom(const std::string& target);

	const std::string& getName() const { return _name; }
protected:
	std::string _name;
};

#define EVENT_APPLICATION_WILL_RESIGN_ACTIVE "Application_willResignActive"
#define EVENT_APPLICATION_DID_BECOME_ACTIVE "Application_didBecomeActive"

NS_AX_END

#endif // __AX_EVENT_H__