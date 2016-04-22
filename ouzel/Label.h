// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Widget.h"
#include "BMFont.h"

namespace ouzel
{
    namespace graphics
    {
        class Texture;
        class MeshBuffer;
        class Shader;
    }

    namespace gui
    {
        class Label: public Widget
        {
        public:
            static std::shared_ptr<Label> create(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            Label();
            virtual ~Label();

            virtual bool init(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));

            virtual void draw() override;

            virtual void setText(const std::string& text);
            virtual const std::string& getText() const { return _text; }

            virtual const graphics::Color& getColor() const { return _color; }
            virtual void setColor(const graphics::Color& color);

        protected:
            void updateMesh();

            graphics::Texture* _texture;
            graphics::MeshBuffer* _meshBuffer;
            graphics::Shader* _shader;
            
            BMFont _font;
            Vector2 _textAnchor;
            std::string _text;

            graphics::Color _color = graphics::Color(255, 255, 255, 255);
        };
    } // namespace gui
} // namespace ouzel
