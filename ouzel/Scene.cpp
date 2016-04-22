// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Scene.h"
#include "Layer.h"
#include "Camera.h"

namespace ouzel
{
    namespace scene
    {
        Scene::Scene()
        {

        }

        Scene::~Scene()
        {

        }

        void Scene::draw()
        {
            lock();

            if (_reorderLayers)
            {
                std::sort(_layers.begin(), _layers.end(), [](Layer* a, Layer* b) {
                    return a->getOrder() > b->getOrder();
                });

                _reorderLayers = false;
            }

            for (Layer* layer : _layers)
            {
                if (!layer->_remove)
                {
                    layer->draw();
                }
            }

            unlock();
        }

        void Scene::addLayer(Layer* layer)
        {
            if (_locked)
            {
                _layerAddList.insert(layer);
            }
            else if (!hasLayer(layer) && !layer->getScene())
            {
                layer->_remove = false;
                _layers.push_back(layer);
                layer->addToScene(shared_from_this());

                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::removeLayer(Layer* layer)
        {
            if (_locked)
            {
                layer->_remove = true;
                _layerRemoveList.insert(layer);
            }
            else
            {
                std::vector<Layer*>::iterator i = std::find(_layers.begin(), _layers.end(), layer);

                if (i != _layers.end())
                {
                    layer->removeFromScene();
                    _layers.erase(i);
                }
            }
        }

        bool Scene::hasLayer(Layer* layer) const
        {
            std::vector<Layer*>::const_iterator i = std::find(_layers.begin(), _layers.end(), layer);

            return i != _layers.end();
        }

        void Scene::recalculateProjection()
        {
            for (Layer* layer : _layers)
            {
                if (CameraPtr camera = layer->getCamera())
                {
                    camera->recalculateProjection();
                }
            }
        }

        void Scene::reorderLayers()
        {
            _reorderLayers = true;
        }

        void Scene::lock()
        {
            ++_locked;
        }

        void Scene::unlock()
        {
            if (--_locked == 0)
            {
                if (!_layerAddList.empty())
                {
                    for (Layer* layer : _layerAddList)
                    {
                        addLayer(layer);
                    }
                    _layerAddList.clear();
                }

                if (!_layerRemoveList.empty())
                {
                    for (Layer* layer : _layerRemoveList)
                    {
                        removeLayer(layer);
                    }
                    _layerRemoveList.clear();
                }
            }
        }

        Node* Scene::pickNode(const Vector2& position) const
        {
            for (std::vector<Layer*>::const_reverse_iterator i = _layers.rbegin(); i != _layers.rend(); ++i)
            {
                Layer* layer = *i;
                CameraPtr camera = layer->getCamera();

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

        std::set<Node*> Scene::pickNodes(const Rectangle& rectangle) const
        {
            std::set<Node*> result;

            for (std::vector<Layer*>::const_reverse_iterator i = _layers.rbegin(); i != _layers.rend(); ++i)
            {
                // TODO: use projection

                std::set<Node*> nodes = (*i)->pickNodes(rectangle);

                result.insert(nodes.begin(), nodes.end());
            }

            return result;
        }

    } // namespace scene
} // namespace ouzel
