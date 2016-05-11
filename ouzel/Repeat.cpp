// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Repeat.h"

namespace ouzel
{
    namespace scene
    {
        Repeat::Repeat(Animator* pAnimator, uint32_t pCount):
            Animator(pAnimator->getLength() * static_cast<float>(pCount)), animator(pAnimator), count(pCount)
        {
            if (animator) animator->retain();
        }

        Repeat::~Repeat()
        {
            if (animator) animator->release();
        }

        void Repeat::update(float delta)
        {
            if (running)
            {
                if (!animator || animator->getLength() <= 0.0f)
                {
                    currentCount = count;
                    done = true;
                    running = false;
                    currentTime = length;
                    setProgress(1.0f);
                }
                else
                {
                    currentTime += delta;
                    currentCount = static_cast<uint32_t>(currentTime / animator->getLength());

                    if (count == 0 || currentCount < count)
                    {
                        float remainingTime = currentTime - animator->getLength() * currentCount;
                        animator->setProgress(remainingTime / animator->getLength());
                    }
                    else
                    {
                        done = true;
                        running = false;
                        currentTime = length;
                        setProgress(1.0f);
                    }
                }
            }
        }

        void Repeat::start(Node* targetNode)
        {
            Animator::start(targetNode);

            if (animator)
            {
                animator->start(targetNode);
            }
        }

        void Repeat::reset()
        {
            Animator::reset();

            if (animator)
            {
                animator->reset();
            }

            currentCount = 0;
        }
    } // namespace scene
} // namespace ouzel
