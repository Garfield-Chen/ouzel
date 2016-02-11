// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "VideoTexture.h"

namespace ouzel
{
    class RendererOGL;
    
    class VideoTextureOGL: public VideoTexture
    {
        friend RendererOGL;
    public:
        virtual ~VideoTextureOGL();
        
        virtual bool initFromFile(const std::string& filename) override;
        
    private:
        VideoTextureOGL();
    };
}
