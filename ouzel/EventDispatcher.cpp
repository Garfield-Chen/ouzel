// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "EventDispatcher.h"

namespace ouzel
{
    EventDispatcher::EventDispatcher()
    {

    }

    EventDispatcher::~EventDispatcher()
    {

    }

    void EventDispatcher::update()
    {
        while (!eventQueue.empty())
        {
            mutex.lock();

            auto eventPair = eventQueue.front();
            eventQueue.pop();

            mutex.unlock();

            Event* event = eventPair.first;

            switch (eventPair.first->type)
            {
                case Event::Type::KEY_DOWN:
                case Event::Type::KEY_UP:
                case Event::Type::KEY_REPEAT:
                    dispatchKeyboardEvent(static_cast<KeyboardEvent*>(event), eventPair.second);
                    break;

                case Event::Type::MOUSE_DOWN:
                case Event::Type::MOUSE_UP:
                case Event::Type::MOUSE_SCROLL:
                case Event::Type::MOUSE_MOVE:
                    dispatchMouseEvent(static_cast<MouseEvent*>(event), eventPair.second);
                    break;
                case Event::Type::TOUCH_BEGIN:
                case Event::Type::TOUCH_MOVE:
                case Event::Type::TOUCH_END:
                case Event::Type::TOUCH_CANCEL:
                    dispatchTouchEvent(static_cast<TouchEvent*>(event), eventPair.second);
                    break;
                case Event::Type::GAMEPAD_CONNECT:
                case Event::Type::GAMEPAD_DISCONNECT:
                case Event::Type::GAMEPAD_BUTTON_CHANGE:
                    dispatchGamepadEvent(static_cast<GamepadEvent*>(event), eventPair.second);
                    break;
                case Event::Type::WINDOW_SIZE_CHANGE:
                case Event::Type::WINDOW_TITLE_CHANGE:
                case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                    dispatchWindowEvent(static_cast<WindowEvent*>(event), eventPair.second);
                    break;
                case Event::Type::LOW_MEMORY:
                case Event::Type::OPEN_FILE:
                    dispatchSystemEvent(static_cast<SystemEvent*>(event), eventPair.second);
                    break;
                case Event::Type::UI_ENTER_NODE:
                case Event::Type::UI_LEAVE_NODE:
                case Event::Type::UI_PRESS_NODE:
                case Event::Type::UI_RELEASE_NODE:
                case Event::Type::UI_CLICK_NODE:
                case Event::Type::UI_DRAG_NODE:
                    dispatchUIEvent(static_cast<UIEvent*>(event), eventPair.second);
                    break;
            }

            event->release();
        }
    }

    void EventDispatcher::addEventHandler(EventHandler* eventHandler)
    {
        eventHandlers.pushBack(eventHandler);
    }

    void EventDispatcher::removeEventHandler(EventHandler* eventHandler)
    {
        eventHandlers.erase(eventHandler);
    }

    void EventDispatcher::dispatchEvent(Event* event, void* sender)
    {
        std::lock_guard<std::mutex> mutexLock(mutex);

        eventQueue.push(std::make_pair(event, sender));
        event->retain();
    }

    void EventDispatcher::dispatchKeyboardEvent(KeyboardEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->keyboardHandler)
            {
                if (!eventHandler->keyboardHandler(event, sender))
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::dispatchMouseEvent(MouseEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->mouseHandler)
            {
                if (!eventHandler->mouseHandler(event, sender))
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::dispatchTouchEvent(TouchEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->touchHandler)
            {
                if (!eventHandler->touchHandler(event, sender))
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::dispatchGamepadEvent(GamepadEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->gamepadHandler)
            {
                if (!eventHandler->gamepadHandler(event, sender))
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::dispatchWindowEvent(WindowEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->windowHandler)
            {
                if (!eventHandler->windowHandler(event, sender))
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::dispatchSystemEvent(SystemEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->systemHandler)
            {
                if (!eventHandler->systemHandler(event, sender))
                {
                    break;
                }
            }
        }
    }

    void EventDispatcher::dispatchUIEvent(UIEvent* event, void* sender)
    {
        Array<EventHandler> eventHandlersCopy = eventHandlers;

        for (EventHandler* eventHandler : eventHandlersCopy)
        {
            if (eventHandler && eventHandler->uiHandler)
            {
                if (!eventHandler->uiHandler(event, sender))
                {
                    break;
                }
            }
        }
    }
}
