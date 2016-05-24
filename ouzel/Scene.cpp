// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"
#include "Engine.h"

namespace ouzel
{
    namespace scene
    {
        Scene::Scene()
        {

        }

        Scene::~Scene()
        {
            for (Layer* layer : layers)
            {
                layer->release();
            }
        }

        void Scene::draw()
        {
            if (reorder)
            {
                std::sort(layers.begin(), layers.end(), [](Layer* a, Layer* b) {
                    return a->getOrder() > b->getOrder();
                });

                reorder = false;
            }

            Array<Layer> layersCopy = layers;

            for (Layer* layer : layersCopy)
            {
                layer->draw();
            }
        }

        void Scene::addLayer(Layer* layer)
        {
            if (!hasLayer(layer) && !layer->getScene())
            {
                layers.pushBack(layer);
                layer->addToScene(this);

                if (Camera* camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::removeLayer(Layer* layer)
        {
            layers.erase(layer);
            layer->removeFromScene();
        }

        void Scene::removeAllLayers()
        {
            Array<Layer> layersCopy = layers;

            for (Layer* layer : layersCopy)
            {
                layer->removeFromScene();
            }

            layers.clear();
        }

        bool Scene::hasLayer(Layer* layer) const
        {
            std::vector<Layer*>::const_iterator i = std::find(layers.begin(), layers.end(), layer);

            return i != layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (Layer* layer : layers)
            {
                if (Camera* camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::reorderLayers()
        {
            reorder = true;
        }

        Node* Scene::pickNode(const Vector2& position) const
        {
            for (std::vector<Layer*>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                Layer* layer = *i;
                Camera* camera = layer->getCamera();

                if (camera)
                {
                    Vector2 worldPosition = camera->convertScreenToWorld(position);

                    if (Node* result = layer->pickNode(worldPosition))
                    {
                        return result;
                    }
                }
            }

            return nullptr;
        }

        std::set<Node*> Scene::pickNodes(const std::vector<Vector2>& edges) const
        {
            std::set<Node*> result;

            for (std::vector<Layer*>::const_reverse_iterator i = layers.rbegin(); i != layers.rend(); ++i)
            {
                std::set<Node*> nodes = (*i)->pickNodes(edges);

                result.insert(nodes.begin(), nodes.end());
            }

            return result;
        }

    } // namespace scene
} // namespace ouzel
