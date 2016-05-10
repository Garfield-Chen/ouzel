// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Ease: public Animator
        {
        public:
            enum class Type
            {
                IN,
                OUT,
                INOUT
            };

            enum class Func
            {
                SINE,
                QUAD,
                CUBIC,
                QUART,
                QUINT,
                EXPO,
                CIRC,
                BACK,
                ELASTIC,
                BOUNCE
            };

            Ease(Animator* pAnimator, Type pType, Func pFunc);

            virtual void start(Node* targetNode) override;
            virtual void reset() override;

        protected:
            virtual void updateProgress() override;
            
            Animator* animator;
            Type type;
            Func func;
        };
    } // namespace scene
} // namespace ouzel
