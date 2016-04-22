// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include "Noncopyable.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;
        class Node;

        class NodeContainer: public Noncopyable, public std::enable_shared_from_this<NodeContainer>
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            virtual bool addChild(Node* node);
            virtual bool removeChild(Node* node);
            virtual void removeAllChildren();
            virtual bool hasChild(Node* node, bool recursive = false) const;
            virtual const std::vector<Node*>& getChildren() const { return _children; }

        protected:
            void lock();
            void unlock();

            std::vector<Node*> _children;

            std::set<Node*> _nodeAddList;
            std::set<Node*> _nodeRemoveList;
            int32_t _locked = 0;
        };
    } // namespace scene
} // namespace ouzel
