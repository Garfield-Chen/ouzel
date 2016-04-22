// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Repeat: public Animator
        {
        public:
            Repeat(Animator* animator, uint32_t count = 0);

            virtual void update(float delta) override;

            virtual void start(Node* node) override;
            virtual void reset() override;

        protected:
            Animator* _animator;
            uint32_t _count = 0;
            uint32_t _currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
