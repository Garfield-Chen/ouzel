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
            for (Node* node : children)
            {
                node->parent = nullptr;
                node->layer = nullptr;
            }
        }

        bool NodeContainer::addChild(Node* node)
        {
            if (locked)
            {
                nodeAddList.insert(node);
                return false;
            }

            if (!hasChild(node) && !node->hasParent())
            {
                node->remove = false;
                node->parent = this;
                children.push_back(node);

                return true;
            }
            else
            {
                return false;
            }
        }

        bool NodeContainer::removeChild(Node* node)
        {
            if (locked)
            {
                node->remove = true;
                nodeRemoveList.insert(node);
                return false;
            }

            std::vector<Node*>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                node->removeFromLayer();
                node->parent = nullptr;
                node->layer = nullptr;
                children.erase(i);

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

            for (auto& node : children)
            {
                node->removeFromLayer();
                node->parent = nullptr;
                node->layer = nullptr;
            }

            children.clear();

            unlock();
        }

        bool NodeContainer::hasChild(Node* node, bool recursive) const
        {
            for (std::vector<Node*>::const_iterator i = children.begin(); i != children.end(); ++i)
            {
                Node* child = *i;

                if (child == node || (recursive && child->hasChild(node, true)))
                {
                    return true;
                }
            }

            return false;
        }

        void NodeContainer::lock()
        {
            ++locked;
        }

        void NodeContainer::unlock()
        {
            if (--locked == 0)
            {
                if (!nodeAddList.empty())
                {
                    for (Node* node : nodeAddList)
                    {
                        addChild(node);
                    }
                    nodeAddList.clear();
                }

                if (!nodeRemoveList.empty())
                {
                    for (Node* node : nodeRemoveList)
                    {
                        removeChild(node);
                    }
                    nodeRemoveList.clear();
                }
            }
        }
    } // namespace scene
} // namespace ouzel
