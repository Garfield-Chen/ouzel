// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Texture.h"

namespace ouzel
{
    class Renderer;
    
    class VideoTexture: public Texture
    {
        friend Renderer;
    public:
        virtual ~VideoTexture();
        
        virtual bool initFromFile(const std::string& filename);
        
    protected:
        VideoTexture();
    };
}
