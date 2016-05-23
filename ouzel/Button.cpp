// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <functional>
#include "Button.h"
#include "Engine.h"
#include "Sprite.h"
#include "EventHander.h"
#include "EventDispatcher.h"
#include "Layer.h"
#include "Camera.h"
#include "Label.h"
#include "Utils.h"
#include "TextDrawable.h"

namespace ouzel
{
    namespace gui
    {
        Button* Button::create(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage, const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {
            Button* result = new Button();

            if (!result->init(normalImage, selectedImage, pressedImage, disabledImage, label, labelColor, font))
            {
                result->release();
            }

            return result;
        }

        Button::Button()
        {

        }

        Button::~Button()
        {
            if (normalSprite) normalSprite->release();
            if (selectedSprite) selectedSprite->release();
            if (pressedSprite) pressedSprite->release();
            if (disabledSprite) disabledSprite->release();
            if (labelDrawable) labelDrawable->release();

            sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
            eventHandler->release();
        }

        bool Button::init(const std::string& normalImage, const std::string& selectedImage, const std::string& pressedImage, const std::string& disabledImage,
                          const std::string& label, const graphics::Color& labelColor, const std::string& font)
        {
            eventHandler = new EventHandler(EventHandler::PRIORITY_MAX + 1);

            eventHandler->gamepadHandler = std::bind(&Button::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
            eventHandler->uiHandler = std::bind(&Button::handleUI, this, std::placeholders::_1, std::placeholders::_2);

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
                selectedSprite = scene::Sprite::createFromFile(selectedImage, false);
                if (selectedSprite)
                {
                    addDrawable(selectedSprite);
                }
            }

            if (!pressedImage.empty())
            {
                pressedSprite = scene::Sprite::createFromFile(pressedImage, false);
                if (pressedSprite)
                {
                    addDrawable(pressedSprite);
                }
            }

            if (!disabledImage.empty())
            {
                disabledSprite = scene::Sprite::createFromFile(disabledImage, false);
                if (disabledSprite)
                {
                    addDrawable(disabledSprite);
                }
            }

            if (!label.empty())
            {
                labelDrawable = scene::TextDrawable::create(font, label);
                if (labelDrawable)
                {
                    labelDrawable->setColor(labelColor);
                    addDrawable(labelDrawable);
                }
            }

            updateSprite();

            return true;
        }

        void Button::setEnabled(bool enabled)
        {
            Widget::setEnabled(enabled);

            selected = false;
            pointerOver = false;
            pressed = false;
            receiveInput = enabled;

            updateSprite();
        }

        bool Button::handleGamepad(GamepadEvent* event, void* sender)
        {
            OUZEL_UNUSED(event);
            OUZEL_UNUSED(sender);

            return true;
        }

        bool Button::handleUI(UIEvent* event, void* sender)
        {
            if (!enabled) return true;

            if (sender == this)
            {
                if (event->type == Event::Type::UI_ENTER_NODE)
                {
                    pointerOver = true;
                    updateSprite();
                }
                else if (event->type == Event::Type::UI_LEAVE_NODE)
                {
                    pointerOver = false;
                    updateSprite();
                }
                else if (event->type == Event::Type::UI_PRESS_NODE)
                {
                    pressed = true;
                    updateSprite();
                }
                else if (event->type == Event::Type::UI_RELEASE_NODE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
                else if (event->type == Event::Type::UI_CLICK_NODE)
                {
                    if (pressed)
                    {
                        pressed = false;
                        updateSprite();
                    }
                }
            }

            return true;
        }

        void Button::updateSprite()
        {
            if (normalSprite) normalSprite->setVisible(false);
            if (selectedSprite) selectedSprite->setVisible(false);
            if (pressedSprite) pressedSprite->setVisible(false);
            if (disabledSprite) disabledSprite->setVisible(false);

            if (enabled)
            {
                if (pressed && pointerOver && pressedSprite)
                {
                    pressedSprite->setVisible(true);
                }
                else if (selected && selectedSprite)
                {
                    selectedSprite->setVisible(true);
                }
                else if (normalSprite)
                {
                    normalSprite->setVisible(true);
                }
            }
            else
            {
                if (disabledSprite)
                {
                    disabledSprite->setVisible(true);
                }
                else if (normalSprite)
                {
                    normalSprite->setVisible(true);
                }
            }
        }
    } // namespace gui
} // namespace ouzel
