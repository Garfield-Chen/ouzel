// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Widget.h"
#include "Color.h"

namespace ouzel
{
    namespace scene
    {
        class TextDrawable;
    }

    namespace gui
    {
        class Label: public Widget
        {
        public:
            static Label* create(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            Label();
            virtual ~Label();

            virtual bool init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor = Vector2(0.5f, 0.5f));

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            virtual const graphics::Color& getColor() const { return color; }
            virtual void setColor(const graphics::Color& newColor);

        protected:
            std::string text;
            graphics::Color color = graphics::Color(255, 255, 255, 255);

            scene::TextDrawable* textDrawable = nullptr;
        };
    } // namespace gui
} // namespace ouzel
