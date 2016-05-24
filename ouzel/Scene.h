// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include <cstdint>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Array.h"
#include "Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;
        class Node;

        class Scene: public Noncopyable, public ReferenceCounted
        {
        public:
            Scene();
            virtual ~Scene();

            virtual void draw();

            void addLayer(Layer* layer);
            void removeLayer(Layer* layer);
            void removeAllLayers();
            bool hasLayer(Layer* layer) const;
            const Array<Layer>& getLayers() const { return layers; }

            virtual void recalculateProjection();

            virtual void reorderLayers();

            Node* pickNode(const Vector2& position) const;
            std::set<Node*> pickNodes(const std::vector<Vector2>& edges) const;

        protected:
            Array<Layer> layers;
            bool reorder = false;
        };
    } // namespace scene
} // namespace ouzel
