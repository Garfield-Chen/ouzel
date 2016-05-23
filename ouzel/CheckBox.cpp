// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CheckBox.h"
#include "Engine.h"
#include "Sprite.h"
#include "TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        CheckBox* CheckBox::create(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            CheckBox* result = new CheckBox();

            if (!result->init(normalImage, selectedImage, pressedImage, disabledImage, tickImage))
            {
                result = nullptr;
            }

            return result;
        }

        CheckBox::CheckBox()
        {

        }

        CheckBox::~CheckBox()
        {
            if (normalSprite) normalSprite->release();
            if (selectedSprite) selectedSprite->release();
            if (pressedSprite) pressedSprite->release();
            if (disabledSprite) disabledSprite->release();
            if (tickSprite) tickSprite->release();

            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
            eventHandler->release();
        }

        bool CheckBox::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& tickImage)
        {
            eventHandler = new EventHandler(EventHandler::PRIORITY_MAX + 1);

            eventHandler->gamepadHandler = std::bind(&CheckBox::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler->uiHandler = std::bind(&CheckBox::handleUI, this, std::placeholders::_1, std::placeholders::_2);

            sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

            if (!normalImage.empty())
            {
                normalSprite = new scene::Sprite();
                if (normalSprite->initFromFile(normalImage, false))
                {
                    addDrawable(normalSprite);
                }
            }

            if (!selectedImage.empty())
            {
                selectedSprite = new scene::Sprite();
                if (selectedSprite->initFromFile(selectedImage, false))
                {
                    addDrawable(selectedSprite);
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite = new scene::Sprite();
                if (pressedSprite->initFromFile(pressedImage, false))
                {
                    addDrawable(pressedSprite);
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite = new scene::Sprite();
                if (disabledSprite->initFromFile(disabledImage, false))
                {
                    addDrawable(disabledSprite);
                }
            }

            if (!tickImage.empty())
            {
                tickSprite = new scene::Sprite();
                if (tickSprite->initFromFile(tickImage, false))
                {
                    addDrawable(tickSprite);
                }
            }

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
