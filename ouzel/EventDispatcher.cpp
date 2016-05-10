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
        lock();

        while (!eventQueue.empty())
        {
            mutex.lock();

            auto eventPair = eventQueue.front();
            eventQueue.pop();

            mutex.unlock();

            switch (eventPair.first->type)
            {
                case Event::Type::KEY_DOWN:
                case Event::Type::KEY_UP:
                case Event::Type::KEY_REPEAT:
                    dispatchKeyboardEvent(static_cast<KeyboardEvent*>(eventPair.first), eventPair.second);
                    break;

                case Event::Type::MOUSE_DOWN:
                case Event::Type::MOUSE_UP:
                case Event::Type::MOUSE_SCROLL:
                case Event::Type::MOUSE_MOVE:
                    dispatchMouseEvent(static_cast<MouseEvent*>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::TOUCH_BEGIN:
                case Event::Type::TOUCH_MOVE:
                case Event::Type::TOUCH_END:
                case Event::Type::TOUCH_CANCEL:
                    dispatchTouchEvent(static_cast<TouchEvent*>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::GAMEPAD_CONNECT:
                case Event::Type::GAMEPAD_DISCONNECT:
                case Event::Type::GAMEPAD_BUTTON_CHANGE:
                    dispatchGamepadEvent(static_cast<GamepadEvent*>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::WINDOW_SIZE_CHANGE:
                case Event::Type::WINDOW_TITLE_CHANGE:
                case Event::Type::WINDOW_FULLSCREEN_CHANGE:
                    dispatchWindowEvent(static_cast<WindowEvent*>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::LOW_MEMORY:
                case Event::Type::OPEN_FILE:
                    dispatchSystemEvent(static_cast<SystemEvent*>(eventPair.first), eventPair.second);
                    break;
                case Event::Type::UI_ENTER_NODE:
                case Event::Type::UI_LEAVE_NODE:
                case Event::Type::UI_PRESS_NODE:
                case Event::Type::UI_RELEASE_NODE:
                case Event::Type::UI_CLICK_NODE:
                case Event::Type::UI_DRAG_NODE:
                    dispatchUIEvent(static_cast<UIEvent*>(eventPair.first), eventPair.second);
                    break;
            }
        }

        unlock();
    }

    void EventDispatcher::addEventHandler(EventHandler* eventHandler)
    {
        if (locked)
        {
            eventHandlerAddList.insert(eventHandler);
        }
        else
        {
            std::vector<EventHandler*>::iterator i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

            if (i == eventHandlers.end())
            {
                eventHandler->remove = false;
                eventHandlers.push_back(eventHandler);

                std::sort(eventHandlers.begin(), eventHandlers.end(), [](EventHandler* a, EventHandler* b) {
                    return a->priority < b->priority;
                });
            }
        }
    }

    void EventDispatcher::removeEventHandler(EventHandler* eventHandler)
    {
        if (locked)
        {
            eventHandler->remove = true;
            eventHandlerRemoveList.insert(eventHandler);
        }
        else
        {
            std::vector<EventHandler*>::iterator i = std::find(eventHandlers.begin(), eventHandlers.end(), eventHandler);

            if (i != eventHandlers.end())
            {
                eventHandlers.erase(i);
            }
        }
    }

    void EventDispatcher::dispatchEvent(Event* event, void* sender)
    {
        std::lock_guard<std::mutex> mutexLock(mutex);

        eventQueue.push(std::make_pair(event, sender));
    }

    void EventDispatcher::dispatchKeyboardEvent(KeyboardEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->keyboardHandler)
            {
                if (!eventHandler->keyboardHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchMouseEvent(MouseEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->mouseHandler)
            {
                if (!eventHandler->mouseHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchTouchEvent(TouchEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->touchHandler)
            {
                if (!eventHandler->touchHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchGamepadEvent(GamepadEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->gamepadHandler)
            {
                if (!eventHandler->gamepadHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchWindowEvent(WindowEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->windowHandler)
            {
                if (!eventHandler->windowHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchSystemEvent(SystemEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->systemHandler)
            {
                if (!eventHandler->systemHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::dispatchUIEvent(UIEvent* event, void* sender)
    {
        lock();

        for (EventHandler* eventHandler : eventHandlers)
        {
            if (eventHandler && !eventHandler->remove && eventHandler->uiHandler)
            {
                if (!eventHandler->uiHandler(event, sender))
                {
                    break;
                }
            }
        }

        unlock();
    }

    void EventDispatcher::lock()
    {
        ++locked;
    }

    void EventDispatcher::unlock()
    {
        if (--locked == 0)
        {
            if (!eventHandlerAddList.empty())
            {
                for (EventHandler* eventHandler : eventHandlerAddList)
                {
                    addEventHandler(eventHandler);
                }
                eventHandlerAddList.clear();
            }

            if (!eventHandlerRemoveList.empty())
            {
                for (EventHandler* eventHandler : eventHandlerRemoveList)
                {
                    removeEventHandler(eventHandler);
                }
                eventHandlerRemoveList.clear();
            }
        }
    }
}
