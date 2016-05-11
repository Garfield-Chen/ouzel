// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Parallel: public Animator
        {
        public:
            Parallel(const std::vector<Animator*>& pAnimators);
            virtual ~Parallel();

            virtual void start(Node* targetNode) override;
            virtual void reset() override;

        protected:
            virtual void updateProgress() override;

            std::vector<Animator*> animators;
        };
    } // namespace scene
} // namespace ouzel
