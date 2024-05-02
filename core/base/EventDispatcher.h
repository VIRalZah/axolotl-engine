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

#ifndef __AX_EVENT_DISPATCHER_H__
#define __AX_EVENT_DISPATCHER_H__

#include "cocoa/Object.h"
#include "Event.h"
#include <vector>

NS_AX_BEGIN

class EventDispatcher : public Object
{
public:
	EventDispatcher();
	virtual ~EventDispatcher();

	virtual void dispatchEvent(Event* event);

	virtual void addHandler(Handler* handler);
	virtual void removeHandler(Handler* handler);

	virtual void setDispatchEvents(bool enabled) { _dispatchEvents = enabled; }
	virtual bool isDispatchEvents() const { return _dispatchEvents; };
protected:
	bool _dispatchEvents;
	bool _locked;
	
	Array* _handlers;
};

NS_AX_END

#endif // __AX_EVENT_DISPATCHER_H__