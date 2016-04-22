// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include <cstdint>
#include "Noncopyable.h"
#include "Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;

        class Scene: public Noncopyable, public std::enable_shared_from_this<Scene>
        {
        public:
            Scene();
            virtual ~Scene();

            virtual void draw();

            void addLayer(Layer* layer);
            void removeLayer(Layer* layer);
            bool hasLayer(Layer* layer) const;
            const std::vector<Layer*>& getLayers() const { return _layers; }

            virtual void recalculateProjection();

            virtual void reorderLayers();

            Node* pickNode(const Vector2& position) const;
            std::set<Node*> pickNodes(const Rectangle& rectangle) const;

        protected:
            void lock();
            void unlock();

            std::vector<Layer*> _layers;
            bool _reorderLayers = false;

            std::set<Layer*> _layerAddList;
            std::set<Layer*> _layerRemoveList;
            int32_t _locked = 0;
        };
    } // namespace scene
} // namespace ouzel
