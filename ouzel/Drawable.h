// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "AABB2.h"
#include "Matrix4.h"
#include "Color.h"

namespace ouzel
{
    namespace scene
    {
        class Node;
        
        class Drawable: public ouzel::Noncopyable, public ReferenceCounted
        {
        public:
            virtual ~Drawable();

            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor);

            virtual const AABB2& getBoundingBox() const { return boundingBox; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool shapeOverlaps(const std::vector<Vector2>& edges) const;

            bool isVisible() const { return visible; }
            virtual void setVisible(bool newVisible) { visible = newVisible; }

            void setParentNode(Node* newParentNode) { parentNode = newParentNode; }
            Node* getParentNode() const { return parentNode; }

        protected:
            AABB2 boundingBox;
            bool visible = true;

            Node* parentNode = nullptr;
        };
    } // namespace scene
} // namespace ouzel
