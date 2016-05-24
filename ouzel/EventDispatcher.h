// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include <mutex>
#include <queue>
#include <cstdint>
#include "Noncopyable.h"
#include "Array.h"
#include "Event.h"
#include "EventHander.h"

namespace ouzel
{
    class EventHandler;

    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~EventDispatcher();

        void update();

        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);

        void dispatchEvent(Event* event, void* sender);

    protected:
        EventDispatcher();

        void dispatchKeyboardEvent(KeyboardEvent* event, void* sender);
        void dispatchMouseEvent(MouseEvent* event, void* sender);
        void dispatchTouchEvent(TouchEvent* event, void* sender);
        void dispatchGamepadEvent(GamepadEvent* event, void* sender);
        void dispatchWindowEvent(WindowEvent* event, void* sender);
        void dispatchSystemEvent(SystemEvent* event, void* sender);
        void dispatchUIEvent(UIEvent* event, void* sender);

        Array<EventHandler> eventHandlers;

        std::queue<std::pair<Event*, void*>> eventQueue;
        std::mutex mutex;
    };
}
