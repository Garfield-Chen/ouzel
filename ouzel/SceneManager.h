// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Noncopyable.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    class Engine;
    
    namespace scene
    {
        class Scene;

        class SceneManager: public Noncopyable
        {
            friend Engine;
        public:
            virtual ~SceneManager();

            void draw();

            void setScene(Scene* newScene);
            Scene* getScene() const { return scene; }

            virtual void recalculateProjection();

        protected:
            SceneManager();

            void lock();
            void unlock();

            Scene* scene = nullptr;

            Scene* nextScene = nullptr;
            int32_t locked = 0;
        };
    } // namespace scene
} // namespace ouzel
