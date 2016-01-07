// Copyright (C) 2015 Elviss Strazdins
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
    
    void EventDispatcher::addEventHandler(std::shared_ptr<EventHandler> const& eventHandler)
    {
        std::vector<std::shared_ptr<EventHandler>>::iterator i = std::find_if(_eventHandlers.begin(), _eventHandlers.end(), [eventHandler](std::shared_ptr<EventHandler> const& p) {
            return p.get() == eventHandler.get();
        });
        
        if (i == _eventHandlers.end())
        {
            _eventHandlers.push_back(eventHandler);
            
            std::sort(_eventHandlers.begin(), _eventHandlers.end(), [](std::shared_ptr<EventHandler> const& a, std::shared_ptr<EventHandler> const& b) {
                return a->_priority < b->_priority;
            });
        }
    }
    
    void EventDispatcher::removeEventHandler(std::shared_ptr<EventHandler> const& eventHandler)
    {
        std::vector<std::shared_ptr<EventHandler>>::iterator i = std::find_if(_eventHandlers.begin(), _eventHandlers.end(), [eventHandler](std::shared_ptr<EventHandler> const& p) {
            return p.get() == eventHandler.get();
        });
        
        if (i != _eventHandlers.end())
        {
            _eventHandlers.erase(i);
        }
    }
    
    void EventDispatcher::dispatchKeyDownEvent(const KeyboardEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->keyDownHandler)
            {
                eventHandler->keyDownHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchKeyUpEvent(const KeyboardEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->keyUpHandler)
            {
                eventHandler->keyUpHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseDownEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseDownHandler)
            {
                eventHandler->mouseDownHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseUpEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseUpHandler)
            {
                eventHandler->mouseUpHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseScrollEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseScrollHandler)
            {
                eventHandler->mouseScrollHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseMoveEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseMoveHandler)
            {
                eventHandler->mouseMoveHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchMouseDragEvent(const MouseEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->mouseDragHandler)
            {
                eventHandler->mouseDragHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchBeginEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->touchBeginHandler)
            {
                eventHandler->touchBeginHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchMoveEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->touchMoveHandler)
            {
                eventHandler->touchMoveHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchEndEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->touchEndHandler)
            {
                eventHandler->touchEndHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchTouchCancelEvent(const TouchEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->keyDownHandler)
            {
                eventHandler->touchCancelHandler(event, sender);
                break;
            }
        }
    }
    
    void EventDispatcher::dispatchScreenSizeEvent(const ScreenSizeEvent& event, std::shared_ptr<void> const& sender)
    {
        for (std::shared_ptr<EventHandler> eventHandler : _eventHandlers)
        {
            if (eventHandler->screenSizeHandler)
            {
                eventHandler->screenSizeHandler(event, sender);
                break;
            }
        }
    }
}