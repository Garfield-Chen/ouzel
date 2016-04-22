// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Engine.h"
#include "Layer.h"
#include "Node.h"
#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
#include "Matrix4.h"

namespace ouzel
{
    namespace scene
    {
        Layer::Layer()
        {

        }

        Layer::~Layer()
        {

        }

        void Layer::draw()
        {
            _drawQueue.clear();

            // render only if there is an active camera
            if (_camera)
            {
                lock();

                for (const Node* child : _children)
                {
                    child->visit(Matrix4::IDENTITY, false);
                }

                std::stable_sort(_drawQueue.begin(), _drawQueue.end(), [](const Node*& a, const Node*& b) {
                    return a->getZ() > b->getZ();
                });

                for (Node* node : _drawQueue)
                {
                    node->process();
                }

                unlock();
            }
        }

        bool Layer::addChild(Node* node)
        {
            if (NodeContainer::addChild(node))
            {
                node->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));

                node->updateTransform(Matrix4::IDENTITY);

                return true;
            }
            else
            {
                return false;
            }
        }

        void Layer::addToDrawQueue(Node* node)
        {
            _drawQueue.push_back(node);
        }

        void Layer::setCamera(const CameraPtr& camera)
        {
            _camera = camera;

            if (_camera)
            {
                _camera->addToLayer(std::static_pointer_cast<Layer>(shared_from_this()));
                _camera->recalculateProjection();
            }
        }

        Node* Layer::pickNode(const Vector2& position) const
        {
            for (std::vector<Node*>::const_reverse_iterator i = _drawQueue.rbegin(); i != _drawQueue.rend(); ++i)
            {
                Node* node = *i;

                if (node->isVisible() && node->isPickable() && node->pointOn(position))
                {
                    return node;
                }
            }

            return nullptr;
        }

        std::set<Node*> Layer::pickNodes(const Rectangle& rectangle) const
        {
            std::set<Node*> result;

            for (std::vector<Node*>::const_reverse_iterator i = _drawQueue.rbegin(); i != _drawQueue.rend(); ++i)
            {
                Node* node = *i;

                if (node->isVisible() && node->isPickable() && node->rectangleOverlaps(rectangle))
                {
                    result.insert(node);
                }
            }

            return result;
        }

        void Layer::setOrder(int32_t order)
        {
            _order = order;

            if (ScenePtr scene = _scene.lock())
            {
                scene->reorderLayers();
            }
        }

        void Layer::addToScene(const ScenePtr& scene)
        {
            _scene = scene;
        }

        void Layer::removeFromScene()
        {
            _scene.reset();
        }
    } // namespace scene
} // namespace ouzel
