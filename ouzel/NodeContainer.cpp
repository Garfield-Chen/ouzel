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
            if (node->hasParent())
            {
                log("Node already has parent");
                node->removeFromParent();
            }

            node->parent = this;
            children.pushBack(node);

            return true;
        }

        bool NodeContainer::removeChild(Node* node)
        {
            children.erase(node);

            return true;
        }

        void NodeContainer::removeAllChildren()
        {
            children.clear();
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
    } // namespace scene
} // namespace ouzel
