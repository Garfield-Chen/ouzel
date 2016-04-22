// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Fade: public Animator
        {
        public:
            Fade(float length, float opacity, bool relative = false);

            virtual void start(Node* node) override;

            virtual void setProgress(float progress) override;

        protected:
            float _opacity;
            float _startOpacity = 0.0f;
            float _targetOpacity = 0.0f;
            float _diff = 0.0f;
            bool _relative;
        };
    } // namespace scene
} // namespace ouzel
