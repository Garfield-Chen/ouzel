// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class Application: public ouzel::Noncopyable
{
public:
    virtual ~Application();

    void begin();

    bool handleKeyboard(ouzel::KeyboardEvent* event, void* sender) const;
    bool handleMouse(ouzel::MouseEvent* event, void* sender) const;
    bool handleTouch(ouzel::TouchEvent* event, void* sender) const;
    bool handleGamepad(ouzel::GamepadEvent* event, void* sender) const;
    bool handleUI(ouzel::UIEvent* event, void* sender) const;

protected:
    ouzel::scene::Layer* rtLayer = nullptr;
    ouzel::scene::Layer* layer = nullptr;
    ouzel::scene::Layer* uiLayer = nullptr;

    ouzel::scene::Node* character = nullptr;
    ouzel::scene::Node* witch = nullptr;
    ouzel::scene::Node* flame = nullptr;

    ouzel::gui::Button* button = nullptr;

    ouzel::EventHandler* eventHandler = nullptr;

    ouzel::graphics::RenderTarget* renderTarget = nullptr;
};
