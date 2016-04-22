// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Noncopyable.h"
#include "Vector2.h"
#include "Rectangle.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager: public Noncopyable
        {
            friend Engine;
        public:
            virtual ~SceneManager();

            void draw();

            void setScene(const ScenePtr& scene);
            const ScenePtr& getScene() const { return _scene; }

            virtual void recalculateProjection();

        protected:
            SceneManager();

            void lock();
            void unlock();

            ScenePtr _scene;

            ScenePtr _newScene;
            int32_t _locked = 0;
        };
    } // namespace scene
} // namespace ouzel
