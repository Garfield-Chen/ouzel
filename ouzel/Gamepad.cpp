// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "Gamepad.h"
#include "Engine.h"
#include "Utils.h"

namespace ouzel
{
    namespace input
    {
        Gamepad::Gamepad()
        {
            memset(buttonStates, 0, sizeof(buttonStates));
        }

        bool Gamepad::isAttached() const
        {
            return false;
        }

        void Gamepad::setAbsoluteDpadValues(bool absoluteDpadValues)
        {
            OUZEL_UNUSED(absoluteDpadValues);
        }

        bool Gamepad::isAbsoluteDpadValues() const
        {
            return false;
        }

        int32_t Gamepad::getPlayerIndex() const
        {
            return -1;
        }

        bool Gamepad::setPlayerIndex(int32_t playerIndex)
        {
            OUZEL_UNUSED(playerIndex);

            return false;
        }

        const GamepadButtonState& Gamepad::getButtonState(GamepadButton button) const
        {
            return buttonStates[static_cast<uint32_t>(button)];
        }

        void Gamepad::handleButtonValueChange(GamepadButton button, bool pressed, float value)
        {
            GamepadEvent* event = new GamepadEvent();
            event->type = Event::Type::GAMEPAD_BUTTON_CHANGE;
            event->gamepad = this;
            event->button = button;
            event->pressed = pressed;
            event->value = value;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, sharedEngine->getInput());
            event->release();

            buttonStates[static_cast<uint32_t>(button)].pressed = pressed;
            buttonStates[static_cast<uint32_t>(button)].value = value;
        }

        void Gamepad::setVibration(Motor motor, float speed)
        {
            OUZEL_UNUSED(motor);
            OUZEL_UNUSED(speed);
        }

        float Gamepad::getVibration(Motor motor)
        {
            OUZEL_UNUSED(motor);

            return 0.0f;
        }
    } // namespace input
} // namespace ouzel
