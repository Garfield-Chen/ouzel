// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Widget.h"

namespace ouzel
{
    struct GamepadEvent;
    struct UIEvent;
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
            static CheckBox* create(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage);

            CheckBox();
            virtual ~CheckBox();

            virtual bool init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage);

            virtual void setEnabled(bool enabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

        protected:
            bool handleGamepad(GamepadEvent* event, void* sender);
            bool handleUI(UIEvent* event, void* sender);

            void updateSprite();

            scene::Sprite* normalSprite = nullptr;
            scene::Sprite* selectedSprite = nullptr;
            scene::Sprite* pressedSprite = nullptr;
            scene::Sprite* disabledSprite = nullptr;
            scene::Sprite* tickSprite = nullptr;

            EventHandler* eventHandler = nullptr;

            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
