// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "Input.h"
#include "Engine.h"
#include "EventDispatcher.h"
#include "Scene.h"
#include "Node.h"
#include "Utils.h"

namespace ouzel
{
    namespace input
    {
        Input::Input()
        {
            memset(keyboardKeyStates, 0, sizeof(keyboardKeyStates));
            memset(mouseButtonStates, 0, sizeof(mouseButtonStates));
        }

        Input::~Input()
        {
            if (mouseNode) mouseNode->release();
            if (mouseDownNode) mouseDownNode->release();
        }

        void Input::update()
        {

        }

        void Input::setCursorVisible(bool visible)
        {
            OUZEL_UNUSED(visible);
        }

        bool Input::isCursorVisible() const
        {
            return false;
        }

        void Input::startGamepadDiscovery()
        {

        }

        void Input::stopGamepadDiscovery()
        {

        }

        void Input::keyDown(KeyboardKey key, uint32_t modifiers)
        {
            KeyboardEvent* event = new KeyboardEvent();
            event->key = key;
            event->modifiers = modifiers;

            if (!keyboardKeyStates[static_cast<uint32_t>(key)])
            {
                keyboardKeyStates[static_cast<uint32_t>(key)] = true;

                event->type = Event::Type::KEY_DOWN;
                sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            }
            else
            {
                event->type = Event::Type::KEY_REPEAT;
                sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            }

            event->release();
        }

        void Input::keyUp(KeyboardKey key, uint32_t modifiers)
        {
            keyboardKeyStates[static_cast<uint32_t>(key)] = false;

            KeyboardEvent* event = new KeyboardEvent();
            event->type = Event::Type::KEY_UP;
            event->key = key;
            event->modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();
        }

        void Input::mouseDown(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = true;

            MouseEvent* event = new MouseEvent();
            event->type = Event::Type::MOUSE_DOWN;
            event->button = button;
            event->position = position;
            event->modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();

            if (scene::Scene* scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::Node* node = scene->pickNode(position);
                mouseDownOnNode(node, position);
            }
        }

        void Input::mouseUp(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = false;

            MouseEvent* event = new MouseEvent();
            event->type = Event::Type::MOUSE_UP;
            event->button = button;
            event->position = position;
            event->modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();

            if (scene::Scene* scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::Node* node = scene->pickNode(position);
                mouseUpOnNode(node, position);
            }
        }

        void Input::mouseMove(const Vector2& position, uint32_t modifiers)
        {
            cursorPosition = position;

            MouseEvent* event = new MouseEvent();
            event->type = Event::Type::MOUSE_MOVE;
            event->position = position;
            event->modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();

            if (scene::Scene* scene = sharedEngine->getSceneManager()->getScene())
            {
                scene::Node* node = scene->pickNode(position);
                mouseEnterNode(node, position);
            }

            if (mouseDownNode)
            {
                mouseDragNode(mouseDownNode, position);
            }
        }

        void Input::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            MouseEvent* event = new MouseEvent();
            event->type = Event::Type::MOUSE_SCROLL;
            event->position = position;
            event->scroll = scroll;
            event->modifiers = modifiers;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();
        }

        void Input::touchBegin(uint64_t touchId, const Vector2& position)
        {
            TouchEvent* event = new TouchEvent();
            event->type = Event::Type::TOUCH_BEGIN;
            event->touchId = touchId;
            event->position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();
        }

        void Input::touchEnd(uint64_t touchId, const Vector2& position)
        {
            TouchEvent* event = new TouchEvent();
            event->type = Event::Type::TOUCH_END;
            event->touchId = touchId;
            event->position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();
        }

        void Input::touchMove(uint64_t touchId, const Vector2& position)
        {
            TouchEvent* event = new TouchEvent();
            event->type = Event::Type::TOUCH_MOVE;
            event->touchId = touchId;
            event->position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();
        }

        void Input::touchCancel(uint64_t touchId, const Vector2& position)
        {
            TouchEvent* event = new TouchEvent();
            event->type = Event::Type::TOUCH_CANCEL;
            event->touchId = touchId;
            event->position = position;

            sharedEngine->getEventDispatcher()->dispatchEvent(event, this);
            event->release();
        }

        void Input::mouseEnterNode(scene::Node* node, const Vector2& position)
        {
            if (mouseNode)
            {
                if (mouseNode == node)
                {
                    return;
                }
                else
                {
                    mouseLeaveNode(mouseNode, position);
                }
            }

            mouseNode = node;

            if (node && node->isReceivingInput())
            {
                MouseEvent* enterEvent = new MouseEvent();
                enterEvent->type = Event::Type::UI_ENTER_NODE;
                enterEvent->position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(enterEvent, node);
                enterEvent->release();
            }
        }

        void Input::mouseLeaveNode(scene::Node* node, const Vector2& position)
        {
            if (node && node->isReceivingInput())
            {
                MouseEvent* leaveEvent = new MouseEvent();
                leaveEvent->type = Event::Type::UI_LEAVE_NODE;
                leaveEvent->position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(leaveEvent, node);
                leaveEvent->release();
            }
        }

        void Input::mouseDownOnNode(scene::Node* node, const Vector2& position)
        {
            mouseDownNode = node;

            if (mouseDownNode && mouseDownNode->isReceivingInput())
            {
                mouseDownNode->retain();

                MouseEvent* enterEvent = new MouseEvent();
                enterEvent->type = Event::Type::UI_PRESS_NODE;
                enterEvent->position = node->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(enterEvent, node);
                enterEvent->release();
            }
        }

        void Input::mouseUpOnNode(scene::Node* node, const Vector2& position)
        {
            if (mouseDownNode && mouseDownNode->isReceivingInput())
            {
                MouseEvent* pressEvent = new MouseEvent();
                pressEvent->type = Event::Type::UI_RELEASE_NODE;
                pressEvent->position = mouseDownNode->convertWorldToLocal(position);

                sharedEngine->getEventDispatcher()->dispatchEvent(pressEvent, mouseDownNode);
                pressEvent->release();

                if (mouseDownNode == node)
                {
                    MouseEvent* clickEvent = new MouseEvent();
                    clickEvent->type = Event::Type::UI_CLICK_NODE;
                    clickEvent->position = mouseDownNode->convertWorldToLocal(position);

                    sharedEngine->getEventDispatcher()->dispatchEvent(clickEvent, mouseDownNode);
                    clickEvent->release();
                }

                mouseDownNode->release();
                mouseDownNode = nullptr;
            }
        }

        void Input::mouseDragNode(scene::Node* node, const Vector2& position)
        {
            MouseEvent* enterEvent = new MouseEvent();
            enterEvent->type = Event::Type::UI_DRAG_NODE;
            enterEvent->position = node->convertWorldToLocal(position);

            sharedEngine->getEventDispatcher()->dispatchEvent(enterEvent, node);
            enterEvent->release();
        }

    } // namespace input
} // namespace ouzel
