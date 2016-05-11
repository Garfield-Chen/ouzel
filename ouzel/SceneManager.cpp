// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SceneManager.h"
#include "Scene.h"

namespace ouzel
{
    namespace scene
    {
        SceneManager::SceneManager()
        {

        }

        SceneManager::~SceneManager()
        {
            if (nextScene)
            {
                nextScene->release();
            }
            if (scene)
            {
                scene->release();
            }
        }

        void SceneManager::setScene(Scene* newScene)
        {
            if (scene != newScene)
            {
                if (locked)
                {
                    if (nextScene)
                    {
                        nextScene->release();
                    }

                    nextScene = newScene;
                    nextScene->retain();
                }
                else
                {
                    if (scene)
                    {
                        scene->release();
                    }

                    scene = newScene;

                    if (scene)
                    {
                        scene->retain();
                        scene->recalculateProjection();
                    }
                }
            }
        }

        void SceneManager::draw()
        {
            if (scene)
            {
                lock();

                scene->draw();

                unlock();
            }
        }

        void SceneManager::recalculateProjection()
        {
            if (scene)
            {
                scene->recalculateProjection();
            }
        }

        void SceneManager::lock()
        {
            ++locked;
        }

        void SceneManager::unlock()
        {
            if (--locked == 0 && nextScene)
            {
                setScene(nextScene);
                nextScene->release();
            }
        }
    } // namespace scene
} // namespace ouzel
