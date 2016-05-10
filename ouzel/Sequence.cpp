// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    namespace scene
    {
        Sequence::Sequence(const std::vector<Animator*>& pAnimators):
            Animator(std::accumulate(pAnimators.begin(), pAnimators.end(), 0.0f, [](float a, Animator* b) { return a + b->getLength(); })), animators(pAnimators)
        {

        }

        void Sequence::start(Node* node)
        {
            Animator::start(node);

            for (auto& animator : animators)
            {
                animator->start(node);
            }
        }

        void Sequence::reset()
        {
            Animator::reset();

            for (auto& animator : animators)
            {
                animator->reset();
            }
        }

        void Sequence::updateProgress()
        {
            Animator::updateProgress();

            float time = 0.0f;

            for (auto& animator : animators)
            {
                float animationLength = animator->getLength();

                if (animationLength <= 0.0f || currentTime > time + animationLength)
                {
                    animator->setProgress(1.0f);
                }
                else if (currentTime <= time)
                {
                    animator->setProgress(0.0f);
                }
                else
                {
                    animator->setProgress((currentTime - time) / animationLength);
                }

                time += animator->getLength();
            }
        }
    } // namespace scene
} // namespace ouzel
