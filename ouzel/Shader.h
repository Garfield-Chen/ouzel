// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

namespace ouzel
{
    namespace graphics
    {
        class Renderer;

        class Shader: public Noncopyable, public ReferenceCounted
        {
            friend Renderer;
        public:
            struct ConstantInfo
            {
                std::string name;
                uint32_t size;
            };

            virtual ~Shader();
            virtual void free() {}

            virtual bool initFromFiles(const std::string& newPixelShader,
                                       const std::string& newVertexShader,
                                       uint32_t newVertexAttributes,
                                       const std::string& pixelShaderFunction = "",
                                       const std::string& vertexShaderFunction = "");
            virtual bool initFromBuffers(const uint8_t* newPixelShader,
                                         uint32_t newPixelShaderSize,
                                         const uint8_t* newVertexShader,
                                         uint32_t newVertexShaderSize,
                                         uint32_t newVertexAttributes,
                                         const std::string& pixelShaderFunction = "",
                                         const std::string& vertexShaderFunction = "");

            uint32_t getVertexAttributes() const { return vertexAttributes; }

            virtual bool setPixelShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0);
            virtual bool setVertexShaderConstantInfo(const std::vector<ConstantInfo>& constantInfo, uint32_t alignment = 0);
            
            virtual bool setPixelShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value);
            virtual bool setVertexShaderConstant(uint32_t index, uint32_t size, uint32_t count, const float* value);

        protected:
            Shader();

            std::string pixelShaderFilename;
            std::string vertexShaderFilename;

            uint32_t vertexAttributes;

            std::vector<ConstantInfo> pixelShaderConstantInfo;
            uint32_t pixelShaderAlignment = 0;
            std::vector<ConstantInfo> vertexShaderConstantInfo;
            uint32_t vertexShaderAlignment = 0;
        };
    } // namespace graphics
} // namespace ouzel
