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

#include "EventDispatcher.h"
#include "Touch.h"
#include "Array.h"
#include <map>

NS_AX_BEGIN

typedef std::pair<int, Touch*> TouchPair;
typedef std::map<int, Touch*> TouchMap;
static TouchMap _touches;

EventDispatcher::EventDispatcher()
	: _dispatchEvents(true)
	, _locked(false)
{
	_handlers = new Array();
	_handlersToAdd = axArrayNew(4);
	_handlersToRemove = axArrayNew(4);
}

EventDispatcher::~EventDispatcher()
{
	AX_SAFE_RELEASE(_handlers);
	axArrayFree(_handlersToAdd);
	axArrayFree(_handlersToRemove);
}

void EventDispatcher::dispatchEvent(Event* event)
{
	if (_dispatchEvents)
	{
		_locked = true;

		if (auto touchEvent = dynamic_cast<EventTouch*>(event))
		{
			auto& position = touchEvent->getPosition();
			auto& type = touchEvent->getTouchType();
			auto id = touchEvent->getID();

			Touch* touch;

			auto find = _touches.find(id);
			if (find == _touches.end())
			{
				touch = new Touch();
				_touches.emplace(TouchPair(id, touch));
			}
			else
			{
				touch = find->second;
			}

			touch->setTouchInfo(id, position.x, position.y);

			Object* obj;
			AXARRAY_FOREACH(_handlers, obj)
			{
				if (auto handler = dynamic_cast<TouchHandler*>(obj))
				{
					bool claimed = false;

					TouchVector::iterator claimedTouchIt;

					if (type == TouchType::BEGAN)
					{
						if (handler->onTouchBegan && (claimed = handler->onTouchBegan(touch)))
						{
							handler->_claimedTouches.push_back(touch);
						}
					}
					else if (!handler->_claimedTouches.empty() &&
						(claimedTouchIt = std::find(
							handler->_claimedTouches.begin(),
							handler->_claimedTouches.end(),
							touch
						)) != handler->_claimedTouches.end())
					{
						if (type == TouchType::MOVED)
						{
							if (handler->onTouchMoved)
							{
								handler->onTouchMoved(touch);
							}
						}
						else if (type == TouchType::ENDED)
						{
							if (handler->onTouchEnded)
							{
								handler->onTouchEnded(touch);
							}

							handler->_claimedTouches.erase(claimedTouchIt);

							touch->release();
							_touches.erase(id);
						}
					}

					if (claimed)
					{
						break;
					}
				}
			}
		}
		else if (auto keyboardEvent = dynamic_cast<EventKeyboard*>(event))
		{
			auto& eventType = keyboardEvent->getType();
			auto& keyCode = keyboardEvent->getKeyCode();

			Object* obj;
			AXARRAY_FOREACH(_handlers, obj)
			{
				if (auto handler = dynamic_cast<KeyboardHandler*>(obj))
				{
					if (eventType == KeyboardEventType::KEY_DOWN)
					{
						if (handler->onKeyDown) handler->onKeyDown(keyCode);
					}
					else if (eventType == KeyboardEventType::KEY_UP)
					{
						if (handler->onKeyUp) handler->onKeyUp(keyCode);
					}
				}
			}
		}
		else if (auto keypadEvent = dynamic_cast<EventKeypad*>(event))
		{
			auto& eventType = keypadEvent->getType();

			Object* obj;
			AXARRAY_FOREACH(_handlers, obj)
			{
				if (auto handler = dynamic_cast<KeypadHandler*>(obj))
				{
					if (eventType == KeypadEventType::KEY_BACK_CLICKED)
					{
						if (handler->onKeyBackClicked) handler->onKeyBackClicked();
					}
					else if (eventType == KeypadEventType::KEY_MENU_CLICKED)
					{
						if (handler->onKeyMenuClicked) handler->onKeyMenuClicked();
					}
				}
			}
		}
		else if (auto imeEvent = dynamic_cast<EventIME*>(event))
		{
			auto& eventType = imeEvent->getType();

			Object* obj;
			AXARRAY_FOREACH(_handlers, obj)
			{
				if (auto handler = dynamic_cast<IMEHandler*>(obj))
				{
					if (eventType == IMEEventType::INPUT_TEXT)
					{
						if (handler->onInputText) handler->onInputText(imeEvent->getString());
					}
					else if (eventType == IMEEventType::DELETE_BACKWARD)
					{
						if (handler->onDeleteBackward) handler->onDeleteBackward();
					}
				}
			}
		}
		else if (auto customEvent = dynamic_cast<EventCustom*>(event))
		{
			Object* obj;
			AXARRAY_FOREACH(_handlers, obj)
			{
				auto handler = dynamic_cast<CustomHandler*>(obj);
				if (handler && handler->target == customEvent->getName())
				{
					if (handler->callback) handler->callback();
				}
			}
		}

		_locked = false;
	}
	if (_handlersToAdd && _handlersToAdd->num > 0)
	{
		Object* obj;
		AXARRAY_DATA_FOREACH(_handlersToAdd, obj)
		{
			_handlers->insertObject(obj, getIndex(static_cast<Handler*>(obj)));
		}
	}
	if (_handlersToRemove && _handlersToRemove->num > 0)
	{
		Object* obj;
		AXARRAY_DATA_FOREACH(_handlersToRemove, obj)
		{
			_handlers->removeObject(obj);
		}
	}
}

void EventDispatcher::addHandler(Handler* handler)
{
	if (_locked)
	{
		axArrayAppendObjectWithResize(_handlersToAdd, handler);
		return;
	}

	if (!_handlers->containsObject(handler))
	{
		_handlers->insertObject(handler, getIndex(handler));
	}
}

void EventDispatcher::removeHandler(Handler* handler)
{
	if (_locked)
	{
		axArrayAppendObjectWithResize(_handlersToRemove, handler);
		return;
	}

	_handlers->removeObject(handler);
}

int EventDispatcher::getIndex(Handler* handler) const
{
	int ret = 0;
	if (handler)
	{
		Object* obj;
		AXARRAY_FOREACH(_handlers, obj)
		{
			if (static_cast<Handler*>(obj)->priority < handler->priority)
			{
				ret++;
			}
		}
	}
	return ret;
}

NS_AX_END
