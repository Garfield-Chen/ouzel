// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "Widget.h"
#include "Event.h"
#include "Color.h"

namespace ouzel
{
    class EventHandler;

    namespace scene
    {
        class Sprite;
        class TextDrawable;
    }

    namespace gui
    {
        class Button: public Widget
        {
        public:
            static Button* create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                                                  const std::string& label = "", const graphics::Color& labelColor = graphics::Color(255, 255, 255, 255), const std::string& font = "");

            Button();
            virtual ~Button();

            virtual bool init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled,
                              const std::string& label = "", const graphics::Color& labelColor = graphics::Color(255, 255, 255, 255), const std::string& font = "");

            virtual void setEnabled(bool enabled) override;

        protected:
            bool handleGamepad(GamepadEvent* event, void* sender);
            bool handleUI(UIEvent* event, void* sender);

            void updateSprite();

            scene::Sprite* normalSprite = nullptr;
            scene::Sprite* selectedSprite = nullptr;
            scene::Sprite* pressedSprite = nullptr;
            scene::Sprite* disabledSprite = nullptr;
            scene::TextDrawable* labelDrawable = nullptr;

            EventHandler* eventHandler = nullptr;

            bool selected = false;
            bool pointerOver = false;
            bool pressed = false;
        };
    } // namespace gui
} // namespace ouzel
