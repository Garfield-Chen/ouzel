// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include "NodeContainer.h"
#include "Size2.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    namespace graphics
    {
        class RenderTarget;
    }

    namespace scene
    {
        class Camera;
        class Scene;

        class Layer: public NodeContainer
        {
            friend Scene;
        public:
            Layer();
            virtual ~Layer();

            virtual void draw();

            virtual bool addChild(Node* node) override;

            void addToDrawQueue(Node* node);

            Camera* getCamera() const { return camera; }
            void setCamera(Camera* newCamera);

            Node* pickNode(const Vector2& position) const;
            std::set<Node*> pickNodes(const std::vector<Vector2>& edges) const;

            int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder);

            Scene* getScene() const { return scene; }

            void setRenderTarget(graphics::RenderTarget* newRenderTarget);
            graphics::RenderTarget* getRenderTarget() const { return renderTarget; }

        protected:
            virtual void addToScene(Scene* newScene);
            virtual void removeFromScene();

            Camera* camera = nullptr;
            std::vector<Node*> drawQueue;

            Scene* scene = nullptr;
            int32_t order = 0;
            bool remove = false;

            graphics::RenderTarget* renderTarget = nullptr;
        };
    } // namespace scene
} // namespace ouzel
