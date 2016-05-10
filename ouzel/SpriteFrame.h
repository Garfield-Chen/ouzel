// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "Noncopyable.h"
#include "Rectangle.h"
#include "Vertex.h"

namespace ouzel
{
    namespace graphics
    {
        class Texture;
        class MeshBuffer;
    }

    namespace scene
    {
        class SpriteFrame: public Noncopyable
        {
        public:
            static std::vector<SpriteFrame*> loadSpriteFrames(const std::string& filename, bool mipmaps = true);
            static SpriteFrame* create(const Rectangle& rectangle, graphics::Texture* texture, bool rotated, const Size2& sourceSize, const Vector2& sourceOffset, const Vector2& pivot);

            SpriteFrame(Rectangle pRectangle,
                        graphics::MeshBuffer* pMeshBuffer,
                        graphics::Texture* pTexture):
                rectangle(pRectangle),
                meshBuffer(pMeshBuffer),
                texture(pTexture)
            {
            }

            const Rectangle& getRectangle() const { return rectangle; }
            graphics::MeshBuffer* getMeshBuffer() const { return meshBuffer; }
            graphics::Texture* getRexture() const { return texture; }

        protected:
            Rectangle rectangle;
            graphics::MeshBuffer* meshBuffer;
            graphics::Texture* texture;
        };
    } // scene
} // ouzel
