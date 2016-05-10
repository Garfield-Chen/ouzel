// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;
        class Node;

        class NodeContainer: public Noncopyable, public ReferenceCounted
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            virtual bool addChild(Node* node);
            virtual bool removeChild(Node* node);
            virtual void removeAllChildren();
            virtual bool hasChild(Node* node, bool recursive = false) const;
            virtual const std::vector<Node*>& getChildren() const { return children; }

        protected:
            void lock();
            void unlock();

            std::vector<Node*> children;

            std::set<Node*> nodeAddList;
            std::set<Node*> nodeRemoveList;
            int32_t locked = 0;
        };
    } // namespace scene
} // namespace ouzel
