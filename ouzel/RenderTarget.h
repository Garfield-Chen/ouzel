// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <algorithm>
#include "ReferenceCounted.h"
#include "Noncopyable.h"
#include "Size2.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;
        class Texture;

        class RenderTarget: public ReferenceCounted, public Noncopyable
        {
            friend Renderer;
        public:
            virtual ~RenderTarget();

            virtual bool init(const Size2& size, bool depthBuffer);

            Texture* getTexture() const { return _texture; }

        protected:
            RenderTarget();

            Size2 _size;
            bool _depthBuffer = false;

            Texture* _texture;
        };
    } // namespace graphics
} // namespace ouzel
