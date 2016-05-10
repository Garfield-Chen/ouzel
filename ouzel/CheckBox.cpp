// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"
#include "Engine.h"

namespace ouzel
{
    namespace gui
    {
        CheckBox::CheckBox()
        {

        }

        CheckBox::~CheckBox()
        {
            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
        }

        bool CheckBox::init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick)
        {
            eventHandler = new EventHandler(EventHandler::PRIORITY_MAX + 1);

            eventHandler->gamepadHandler = std::bind(&CheckBox::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler->uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

            return true;
        }

        void CheckBox::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);
        }

        void CheckBox::setChecked(bool newChecked)
        {
            checked = newChecked;
        }

        bool CheckBox::handleGamepad(GamepadEvent* event, void* sender)
        {
            return true;
        }

        bool CheckBox::handleUI(UIEvent* event, void* sender)
        {
            return true;
        }

        void CheckBox::updateSprite()
        {
        }
    } // namespace gui
} // namespace ouzel
