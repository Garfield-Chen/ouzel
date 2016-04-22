// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<Animator*>& animators):
            Animator(std::accumulate(animators.begin(), animators.end(), 0.0f, [](float a, Animator* b) { return a + b->getLength(); })), _animators(animators)
        {

        }

        void Sequence::start(Node* node)
        {
            Animator::start(node);

            for (auto& animator : _animators)
            {
                animator->start(node);
            }
        }

        void Sequence::reset()
        {
            Animator::reset();

            for (auto& animator : _animators)
            {
                animator->reset();
            }
        }

        void Sequence::setProgress(float progress)
        {
            Animator::setProgress(progress);

            float time = 0.0f;

            for (auto& animator : _animators)
            {
                float animationLength = animator->getLength();

                if (animationLength <= 0.0f || _currentTime > time + animationLength)
                {
                    animator->setProgress(1.0f);
                }
                else if (_currentTime <= time)
                {
                    animator->setProgress(0.0f);
                }
                else
                {
                    animator->setProgress((_currentTime - time) / animationLength);
                }

                time += animator->getLength();
            }
        }
    } // namespace scene
} // namespace ouzel
