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

#include "Event.h"
#include <codecvt>

NS_AX_BEGIN

// Handler

Handler::Handler()
{
}

Handler::~Handler()
{
}

// Event

Event::Event()
{
}

Event::~Event()
{
}

// EventTouch

EventTouch::EventTouch(Vec2 position, TouchType eventType, int id)
{
	_position = position;
	_type = eventType;
	_id = id;
}

// EventKeyboard

EventKeyboard::EventKeyboard(KeyboardEventType eventType, KeyCode keyCode)
{
	_type = eventType;
	_keyCode = keyCode;
}

// EventKeypad

EventKeypad::EventKeypad(KeypadEventType eventType)
{
	_type = eventType;
}

// EventIME

EventIME::EventIME(const IMEEventType& type)
{
	_type = type;
}

EventIME::EventIME(const IMEEventType& type, const wchar_t* text)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	_utf8Text = converter.to_bytes(text);
	_type = type;
}

EventIME::EventIME(const IMEEventType& type, const char* text)
{
	_utf8Text = text;
	_type = type;
}

// EventCustom

EventCustom::EventCustom(const std::string& target)
{
	_name = target;
}

NS_AX_END