// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Animator.h"
#include "Utils.h"
#include "Engine.h"

namespace ouzel
{
    namespace scene
    {
        Animator::Animator(float length):
            _length(length)
        {
            _updateCallback = std::make_shared<UpdateCallback>();
            _updateCallback->callback = std::bind(&Animator::update, this, std::placeholders::_1);
        }

        Animator::~Animator()
        {
            sharedEngine->unscheduleUpdate(_updateCallback);
        }

        void Animator::update(float delta)
        {
            if (_running)
            {
                if (_currentTime + delta >= _length)
                {
                    _done = true;
                    _running = false;
                    setProgress(1.0f);
                    sharedEngine->unscheduleUpdate(_updateCallback);
                }
                else
                {
                    setProgress((_currentTime + delta) / _length);
                }
            }
        }

        void Animator::start(Node* node)
        {
            if (!_running)
            {
                _running = true;
                _node = node;

                sharedEngine->scheduleUpdate(_updateCallback);
            }
        }

        void Animator::resume()
        {
            if (!_running)
            {
                _running = true;
                sharedEngine->scheduleUpdate(_updateCallback);
            }
        }

        void Animator::stop(bool resetAnimation)
        {
            if (_running)
            {
                _running = false;
                sharedEngine->unscheduleUpdate(_updateCallback);
            }

            if (resetAnimation)
            {
                reset();
            }
        }

        void Animator::reset()
        {
            _done = false;
            _currentTime = 0.0f;
            setProgress(0.0f);
        }

        void Animator::setProgress(float progress)
        {
            _progress = progress;
            _currentTime = _progress * _length;
        }
    } // namespace scene
} // namespace ouzel
