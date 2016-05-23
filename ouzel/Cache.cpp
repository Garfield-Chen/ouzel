// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Cache.h"
#include "Engine.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "ParticleDefinition.h"
#include "SpriteFrame.h"
#include "Utils.h"

namespace ouzel
{
    Cache::Cache()
    {

    }

    Cache::~Cache()
    {

    }

    void Cache::preloadTexture(const std::string& filename, bool dynamic, bool mipmaps)
    {
        std::unordered_map<std::string, graphics::Texture*>::const_iterator i = textures.find(filename);

        if (i == textures.end())
        {
            graphics::Texture* texture = sharedEngine->getRenderer()->loadTextureFromFile(filename, dynamic, mipmaps);

            if (texture)
            {
                textures[filename] = texture;
            }
        }
    }

    graphics::Texture* Cache::getTexture(const std::string& filename, bool dynamic, bool mipmaps) const
    {
        graphics::Texture* result;

        std::unordered_map<std::string, graphics::Texture*>::const_iterator i = textures.find(filename);

        if (i != textures.end())
        {
            return i->second;
        }
        else
        {
            result = sharedEngine->getRenderer()->loadTextureFromFile(filename, dynamic, mipmaps);

            if (result)
            {
                textures[filename] = result;
            }
        }

        return result;
    }

    void Cache::setTexture(const std::string& filename, graphics::Texture* texture)
    {
        textures[filename] = texture;
    }

    void Cache::releaseTextures()
    {
        textures.clear();
    }

    void Cache::preloadSpriteFrames(const std::string& filename, bool mipmaps)
    {
        std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

        std::vector<scene::SpriteFrame*> frames;

        if (extension == "json")
        {
            frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
        }
        else
        {
            graphics::Texture* texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

            if (!texture)
            {
                return;
            }

            Rectangle rectangle(0, 0, texture->getSize().width, texture->getSize().height);

            scene::SpriteFrame* frame = scene::SpriteFrame::create(rectangle, texture, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
            frames.push_back(frame);
        }

        spriteFrames[filename] = frames;
    }

    std::vector<scene::SpriteFrame*> Cache::getSpriteFrames(const std::string& filename, bool mipmaps) const
    {
        std::unordered_map<std::string, std::vector<scene::SpriteFrame*>>::const_iterator i = spriteFrames.find(filename);

        if (i != spriteFrames.end())
        {
            return i->second;
        }
        else
        {
            std::string extension = sharedEngine->getFileSystem()->getExtensionPart(filename);

            std::vector<scene::SpriteFrame*> frames;

            if (extension == "json")
            {
                frames = scene::SpriteFrame::loadSpriteFrames(filename, mipmaps);
            }
            else
            {
                graphics::Texture* texture = sharedEngine->getCache()->getTexture(filename, false, mipmaps);

                if (!texture)
                {
                    return frames;
                }

                Rectangle rectangle(0.0f, 0.0f, texture->getSize().width, texture->getSize().height);

                scene::SpriteFrame* frame = scene::SpriteFrame::create(rectangle, texture, false, texture->getSize(), Vector2(), Vector2(0.5f, 0.5f));
                frames.push_back(frame);
            }

            return frames;
        }
    }

    void Cache::setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFrame*>& frames)
    {
        spriteFrames[filename] = frames;
    }

    void Cache::releaseSpriteFrames()
    {
        spriteFrames.clear();
    }

    graphics::Shader* Cache::getShader(const std::string& shaderName) const
    {
        std::unordered_map<std::string, graphics::Shader*>::const_iterator i = shaders.find(shaderName);

        if (i != shaders.end())
        {
            return i->second;
        }
        else
        {
            return nullptr;
        }
    }

    void Cache::setShader(const std::string& shaderName, graphics::Shader* shader)
    {
        shaders[shaderName] = shader;
    }

    void Cache::preloadParticleDefinition(const std::string& filename)
    {
        std::unordered_map<std::string, scene::ParticleDefinition*>::const_iterator i = particleDefinitions.find(filename);

        if (i == particleDefinitions.end())
        {
            scene::ParticleDefinition* result = scene::ParticleDefinition::loadParticleDefinition(filename);

            if (result)
            {
                particleDefinitions[filename] = result;
            }
        }
    }

    scene::ParticleDefinition* Cache::getParticleDefinition(const std::string& filename) const
    {
        scene::ParticleDefinition* result;

        std::unordered_map<std::string, scene::ParticleDefinition*>::const_iterator i = particleDefinitions.find(filename);

        if (i != particleDefinitions.end())
        {
            return i->second;
        }
        else
        {
            result = scene::ParticleDefinition::loadParticleDefinition(filename);

            if (result)
            {
                particleDefinitions[filename] = result;
            }
        }

        return result;
    }
    

    graphics::BlendState* Cache::getBlendState(const std::string& blendStateName) const
    {
        std::unordered_map<std::string, graphics::BlendState*>::const_iterator i = blendStates.find(blendStateName);

        if (i != blendStates.end())
        {
            return i->second;
        }
        else
        {
            return nullptr;
        }
    }

    void Cache::setBlendState(const std::string& blendStateName, graphics::BlendState* blendState)
    {
        blendStates[blendStateName] = blendState;
    }
}
