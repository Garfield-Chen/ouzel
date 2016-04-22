// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "NodeContainer.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        NodeContainer::NodeContainer()
        {

        }

        NodeContainer::~NodeContainer()
        {
            for (Node* node : _children)
            {
                node->_parent.reset();
                node->_layer.reset();
            }
        }

        bool NodeContainer::addChild(Node* node)
        {
            if (_locked)
            {
                _nodeAddList.insert(node);
                return false;
            }

            if (!hasChild(node) && !node->hasParent())
            {
                node->_remove = false;
                node->_parent = shared_from_this();
                _children.push_back(node);

                return true;
            }
            else
            {
                return false;
            }
        }

        bool NodeContainer::removeChild(Node* node)
        {
            if (_locked)
            {
                node->_remove = true;
                _nodeRemoveList.insert(node);
                return false;
            }

            std::vector<Node*>::iterator i = std::find(_children.begin(), _children.end(), node);

            if (i != _children.end())
            {
                node->removeFromLayer();
                node->_parent.reset();
                node->_layer.reset();
                _children.erase(i);

                return true;
            }
            else
            {
                return false;
            }
        }

        void NodeContainer::removeAllChildren()
        {
            lock();

            for (auto& node : _children)
            {
                node->removeFromLayer();
                node->_parent.reset();
                node->_layer.reset();
            }

            _children.clear();

            unlock();
        }

        bool NodeContainer::hasChild(Node* node, bool recursive) const
        {
            for (std::vector<Node*>::const_iterator i = _children.begin(); i != _children.end(); ++i)
            {
                const Node*& child = *i;

                if (child == node || (recursive && child->hasChild(node, true)))
                {
                    return true;
                }
            }

            return false;
        }

        void NodeContainer::lock()
        {
            ++_locked;
        }

        void NodeContainer::unlock()
        {
            if (--_locked == 0)
            {
                if (!_nodeAddList.empty())
                {
                    for (Node* node : _nodeAddList)
                    {
                        addChild(node);
                    }
                    _nodeAddList.clear();
                }

                if (!_nodeRemoveList.empty())
                {
                    for (Node* node : _nodeRemoveList)
                    {
                        removeChild(node);
                    }
                    _nodeRemoveList.clear();
                }
            }
        }
    } // namespace scene
} // namespace ouzel
