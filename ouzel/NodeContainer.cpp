// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "NodeContainer.h"
#include "Node.h"
#include "Utils.h"

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
                node->release();
            }
        }

        bool NodeContainer::addChild(Node* node)
        {
            if (locked)
            {
                nodeAddList.insert(node);
                node->retain();
                return false;
            }

            if (node->hasParent())
            {
                log("Node already has parent");
                node->removeFromParent();
            }

            node->remove = false;
            node->parent = this;
            children.push_back(node);
            node->retain();

            return true;
        }

        bool NodeContainer::removeChild(Node* node)
        {
            if (locked)
            {
                node->remove = true;
                nodeRemoveList.insert(node);
                node->retain();
                return false;
            }

            std::vector<Node*>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                node->removeFromLayer();
                node->parent = nullptr;
                node->layer = nullptr;
                children.erase(i);
                node->release();

                return true;
            }
            else
            {
                return false;
            }
        }

        void NodeContainer::removeAllChildren()
        {
            if (locked)
            {
                for (Node* node : children)
                {
                    node->remove = true;
                    nodeRemoveList.insert(node);
                    node->retain();
                }
            }
            else
            {
                for (Node* node : children)
                {
                    node->removeFromLayer();
                    node->parent = nullptr;
                    node->layer = nullptr;
                    node->release();
                }

                children.clear();
            }

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
                        node->release();
                    }
                    nodeAddList.clear();
                }

                if (!nodeRemoveList.empty())
                {
                    for (Node* node : nodeRemoveList)
                    {
                        removeChild(node);
                        node->release();
                    }
                    nodeRemoveList.clear();
                }
            }
        }
    } // namespace scene
} // namespace ouzel
