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
    ouzel::scene::Layer* rtLayer;
    ouzel::scene::Layer* layer;
    ouzel::scene::Layer* uiLayer;

    ouzel::scene::Node* character;
    ouzel::scene::Node* witch;
    ouzel::scene::Node* flame;

    ouzel::gui::Button* button;

    ouzel::EventHandler* eventHandler;

    ouzel::graphics::RenderTarget* renderTarget;
};
