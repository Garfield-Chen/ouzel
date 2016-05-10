// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Event.h"

namespace ouzel
{
    namespace input
    {
        struct GamepadButtonState
        {
            bool pressed;
            float value;
        };

        class Gamepad: public Noncopyable, public ReferenceCounted
        {
        public:
            enum class Motor
            {
                ALL = 0,
                LEFT = 1,
                RIGHT = 2
            };

            virtual bool isAttached() const;
            virtual void setAbsoluteDpadValues(bool absoluteDpadValues);
            virtual bool isAbsoluteDpadValues() const;

            virtual int32_t getPlayerIndex() const;
            virtual bool setPlayerIndex(int32_t playerIndex);

            virtual const GamepadButtonState& getButtonState(GamepadButton button) const;

            virtual void setVibration(Motor motor, float speed);
            virtual float getVibration(Motor motor);

        protected:
            Gamepad();

            void handleButtonValueChange(GamepadButton button, bool pressed, float value);

            GamepadButtonState buttonStates[static_cast<uint32_t>(GamepadButton::BUTTON_COUNT)];
        };
    } // namespace input
} // namespace ouzel
