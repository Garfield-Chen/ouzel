// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <unordered_map>
#include "Noncopyable.h"

namespace ouzel
{
    namespace graphics
    {
        class Texture;
        class Shader;
        class BlendState;
    }

    class Cache: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~Cache();

        void preloadTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true);
        graphics::Texture* getTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true) const;
        void setTexture(const std::string& filename, const graphics::Texture* texture);
        virtual void releaseTextures();

        graphics::Shader* getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, const graphics::Shader* shader);

        void preloadParticleDefinition(const std::string& filename);
        scene::ParticleDefinitionPtr getParticleDefinition(const std::string& filename) const;

        graphics::BlendState* getBlendState(const std::string& blendStateName) const;
        void setBlendState(const std::string& blendStateName, const graphics::BlendState* blendState);

    protected:
        Cache();

        scene::ParticleDefinitionPtr loadParticleDefinition(const std::string& filename) const;

        mutable std::unordered_map<std::string, graphics::Texture*> _textures;
        mutable std::unordered_map<std::string, graphics::Shader*> _shaders;
        mutable std::unordered_map<std::string, scene::ParticleDefinitionPtr> _particleDefinitions;
        mutable std::unordered_map<std::string, graphics::BlendState*> _blendStates;
    };
}
