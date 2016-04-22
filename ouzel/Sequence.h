// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Sequence: public Animator
        {
        public:
            Sequence(const std::vector<Animator*>& animators);

            virtual void start(Node* node) override;
            virtual void reset() override;

            virtual void setProgress(float progress) override;

        protected:
            std::vector<Animator*> _animators;
        };
    } // namespace scene
} // namespace ouzel
