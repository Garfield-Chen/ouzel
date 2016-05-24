// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Array.h"

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
            virtual const Array<Node>& getChildren() const { return children; }

        protected:
            Array<Node> children;
        };
    } // namespace scene
} // namespace ouzel
