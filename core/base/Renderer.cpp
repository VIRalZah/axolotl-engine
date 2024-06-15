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

#include "Renderer.h"
#include <algorithm>

NS_AX_BEGIN

Renderer::Renderer()
	: _reorderDirty(false)
{
}

Renderer::~Renderer()
{
	for (const auto& node : _nodes)
	{
		node->release();
	}
	_nodes.clear();
}

void Renderer::attachNode(Node* node)
{
	if (std::find(_nodes.begin(), _nodes.end(), node) == _nodes.end())
	{
		node->retain();

		_nodes.push_back(node);
		_reorderDirty = true;
	}
}

void Renderer::detachNode(Node* node)
{
	auto find = std::find(_nodes.begin(), _nodes.end(), node);
	if (find != _nodes.end())
	{
		(*find)->release();

		_nodes.erase(find);
		_reorderDirty = true;
	}
}

void Renderer::render()
{
	if (_reorderDirty)
	{
		std::sort(_nodes.begin(), _nodes.end(), [](Node* a, Node* b) -> bool
			{
				return a->getZOrder() < b->getZOrder();
			}
		);

		_reorderDirty = false;
	}

	for (const auto& node : _nodes)
	{
		node->visit();
	}
}

NS_AX_END