// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Drawable.h"
#include "Size2.h"
#include "MeshBuffer.h"
#include "Rectangle.h"

namespace ouzel
{
    class UpdateCallback;

    namespace graphics
    {
        class Shader;
        class BlendState;
    }
    
    namespace scene
    {
        class SceneManager;
        class SpriteFrame;

        class Sprite: public Drawable
        {
        public:
            static Sprite* createFromSpriteFrames(const std::vector<SpriteFrame*>& spriteFrames);
            static Sprite* createFromFile(const std::string& filename, bool mipmaps = true);

            Sprite();
            virtual ~Sprite();

            virtual bool initFromSpriteFrames(const std::vector<SpriteFrame*>& spriteFrames);
            virtual bool initFromFile(const std::string& filename, bool mipmaps = true);

            virtual void update(float delta);
            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor) override;

            virtual graphics::Shader* getShader() const { return shader; }
            virtual void setShader(graphics::Shader* newShader);

            virtual const Size2& getSize() const { return size; }

            virtual void play(bool pRepeat = true, float newFrameInterval = 0.1f);
            virtual void stop(bool resetAnimation = true);
            virtual void reset();
            virtual bool isPlaying() const { return playing; }
            
        protected:
            graphics::Shader* shader;
            graphics::BlendState* blendState;

            Size2 size;

            std::vector<SpriteFrame*> frames;

            uint32_t currentFrame = 0;
            float frameInterval = 0.0f;
            bool playing = false;
            bool repeat = false;
            float timeSinceLastFrame = 0.0f;

            UpdateCallback* updateCallback;
        };
    } // namespace scene
} // namespace ouzel
