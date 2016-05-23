// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <algorithm>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Size2.h"
#include "Color.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget: public Noncopyable, public ReferenceCounted
        {
            friend Renderer;
        public:
            virtual ~RenderTarget();
            virtual void free();

            virtual bool init(const Size2& newSize, bool useDepthBuffer);

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            Texture* getTexture() const { return texture; }

        protected:
            RenderTarget();

            Size2 size;
            bool depthBuffer = false;

            Color clearColor;
            Texture* texture = nullptr;
        };
    } // namespace graphics
} // namespace ouzel
