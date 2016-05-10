// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Noncopyable.h"

namespace ouzel
{
    class Engine;

    namespace graphics
    {
        class Texture;
        class Shader;
        class BlendState;
    }

    namespace scene
    {
        class SpriteFrame;
        class ParticleDefinition;
    }
    
    class Cache: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~Cache();

        void preloadTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true);
        graphics::Texture* getTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true) const;
        void setTexture(const std::string& filename, graphics::Texture* texture);
        void releaseTextures();

        void preloadSpriteFrames(const std::string& filename, bool mipmaps = true);
        std::vector<scene::SpriteFrame*> getSpriteFrames(const std::string& filename, bool mipmaps = true) const;
        void setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFrame*>& frames);
        void releaseSpriteFrames();

        graphics::Shader* getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, graphics::Shader* shader);

        void preloadParticleDefinition(const std::string& filename);
        scene::ParticleDefinition* getParticleDefinition(const std::string& filename) const;

        graphics::BlendState* getBlendState(const std::string& blendStateName) const;
        void setBlendState(const std::string& blendStateName, graphics::BlendState* blendState);

    protected:
        Cache();

        mutable std::unordered_map<std::string, graphics::Texture*> textures;
        mutable std::unordered_map<std::string, graphics::Shader*> shaders;
        mutable std::unordered_map<std::string, scene::ParticleDefinition*> particleDefinitions;
        mutable std::unordered_map<std::string, graphics::BlendState*> blendStates;
        mutable std::unordered_map<std::string, std::vector<scene::SpriteFrame*>> spriteFrames;
    };
}
