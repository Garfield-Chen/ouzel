// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Application.h"

class SounSample: public ouzel::scene::Scene
{
public:
    SounSample(Application& app);
    virtual ~SounSample();

    bool handleUI(ouzel::Event::Type type, const ouzel::UIEvent& event, const ouzel::VoidPtr& sender) const;

private:
    Application& application;
    ouzel::gui::ButtonPtr backButton;
    ouzel::gui::ButtonPtr soundButton;
    ouzel::EventHandlerPtr eventHandler;
};