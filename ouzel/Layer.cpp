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
            drawQueue.clear();

            // render only if there is an active camera
            if (camera)
            {
                sharedEngine->getRenderer()->activateRenderTarget(renderTarget);
                
                lock();

                for (Node* child : children)
                {
                    child->visit(Matrix4::IDENTITY, false);
                }

                std::stable_sort(drawQueue.begin(), drawQueue.end(), [](Node* a, Node* b) {
                    return a->getZ() > b->getZ();
                });

                for (Node* node : drawQueue)
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
                node->addToLayer(this);

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
            drawQueue.push_back(node);
        }

        void Layer::setCamera(Camera* newCamera)
        {
            camera = newCamera;

            if (camera)
            {
                camera->addToLayer(this);
                camera->recalculateProjection();
            }
        }

        Node* Layer::pickNode(const Vector2& position) const
        {
            for (std::vector<Node*>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                Node* node = *i;

                if (node->isVisible() && node->isPickable() && node->pointOn(position))
                {
                    return node;
                }
            }

            return nullptr;
        }

        std::set<Node*> Layer::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::set<Node*> result;

            for (std::vector<Node*>::const_reverse_iterator i = drawQueue.rbegin(); i != drawQueue.rend(); ++i)
            {
                Node* node = *i;

                if (node->isVisible() && node->isPickable() && node->shapeOverlaps(edges))
                {
                    result.insert(node);
                }
            }

            return result;
        }

        void Layer::setOrder(int32_t newOrder)
        {
            order = newOrder;

            if (scene)
            {
                scene->reorderLayers();
            }
        }

        void Layer::addToScene(Scene* newScene)
        {
            scene = newScene;
        }

        void Layer::removeFromScene()
        {
            scene->release();
        }

        void Layer::setRenderTarget(graphics::RenderTarget* newRenderTarget)
        {
            renderTarget = newRenderTarget;

            if (camera)
            {
                camera->recalculateProjection();
            }
        }

    } // namespace scene
} // namespace ouzel
