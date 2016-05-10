// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Drawable.h"
#include "Color.h"
#include "BMFont.h"

namespace ouzel
{
    namespace graphics
    {
        class Shader;
    }

    namespace scene
    {
        class TextDrawable: public Drawable
        {
        public:
            static TextDrawable* create(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            TextDrawable();
            virtual ~TextDrawable();

            bool init(const std::string& fontFile, const std::string& newText, const Vector2& newTextAnchor = Vector2(0.5f, 0.5f));

            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor) override;

            virtual void setText(const std::string& newText);
            virtual const std::string& getText() const { return text; }

            virtual const graphics::Color& getColor() const { return color; }
            virtual void setColor(const graphics::Color& newColor);

        protected:
            void updateMesh();

            graphics::Texture* texture;
            graphics::MeshBuffer* meshBuffer;
            graphics::Shader* shader;

            BMFont font;
            Vector2 textAnchor;
            std::string text;

            graphics::Color color = graphics::Color(255, 255, 255, 255);
        };
    } // namespace scene
} // namespace ouzel
