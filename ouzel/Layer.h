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

            const CameraPtr& getCamera() const { return _camera; }
            void setCamera(const CameraPtr& camera);

            Node* pickNode(const Vector2& position) const;
            std::set<Node*> pickNodes(const Rectangle& rectangle) const;

            int32_t getOrder() const { return _order; }
            void setOrder(int32_t order);

            ScenePtr getScene() const { return _scene.lock(); }

        protected:
            virtual void addToScene(const ScenePtr& scene);
            virtual void removeFromScene();

            CameraPtr _camera;
            std::vector<Node*> _drawQueue;

            SceneWeakPtr _scene;
            int32_t _order = 0;
            bool _remove = false;
        };
    } // namespace scene
} // namespace ouzel
