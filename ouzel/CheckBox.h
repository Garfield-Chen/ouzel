// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Widget.h"

namespace ouzel
{
    class GamepadEvent;
    class UIEvent;
    class EventHandler;

    namespace scene
    {
        class Sprite;
        class TextDrawable;
    }

    namespace gui
    {
        class CheckBox: public Widget
        {
        public:
            static CheckBox* create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick);

            CheckBox();
            virtual ~CheckBox();

            virtual bool init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick);

            virtual void setEnabled(bool enabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

        protected:
            bool handleGamepad(GamepadEvent* event, void* sender);
            bool handleUI(UIEvent* event, void* sender);

            void updateSprite();

            scene::Sprite* normalSprite;
            scene::Sprite* selectedSprite;
            scene::Sprite* pressedSprite;
            scene::Sprite* disabledSprite;
            scene::TextDrawable* labelDrawable;

            EventHandler* eventHandler;

            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
