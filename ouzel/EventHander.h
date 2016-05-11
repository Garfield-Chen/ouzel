// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Event.h"

namespace ouzel
{
    class EventDispatcher;

    class EventHandler: public Noncopyable, public ReferenceCounted
    {
        friend EventDispatcher;

    public:
        static const int32_t PRIORITY_MAX = 0x1000;

        EventHandler(int32_t pPriority = 0): priority(pPriority) { }

        std::function<bool(KeyboardEvent*, void*)> keyboardHandler;
        std::function<bool(MouseEvent*, void*)> mouseHandler;
        std::function<bool(TouchEvent*, void*)> touchHandler;
        std::function<bool(GamepadEvent*, void*)> gamepadHandler;
        std::function<bool(WindowEvent*, void*)> windowHandler;
        std::function<bool(SystemEvent*, void*)> systemHandler;
        std::function<bool(UIEvent*, void*)> uiHandler;

    protected:
        int32_t priority;
        bool remove = false;
    };
}
