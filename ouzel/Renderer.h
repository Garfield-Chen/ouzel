// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "Noncopyable.h"
#include "Rectangle.h"
#include "Matrix4.h"
#include "Size2.h"
#include "Color.h"
#include "AABB2.h"
#include "Vertex.h"
#include "Shader.h"
#include "BlendState.h"

namespace ouzel
{
    class Engine;
    class Window;

    namespace scene
    {
        class Camera;
    }

    namespace graphics
    {
        const std::string SHADER_TEXTURE = "shaderTexture";
        const std::string SHADER_COLOR = "shaderColor";

        const std::string BLEND_NO_BLEND = "blendNoBlend";
        const std::string BLEND_ADD = "blendAdd";
        const std::string BLEND_MULTIPLY = "blendMultiply";
        const std::string BLEND_ALPHA = "blendAlpha";

        class MeshBuffer;
        class Texture;
        class RenderTarget;

        class Renderer: public Noncopyable
        {
            friend Engine;
            friend Window;
        public:
            static const uint32_t TEXTURE_LAYERS = 2;

            enum class Driver
            {
                DEFAULT = 0,
                NONE,
                OPENGL,
                DIRECT3D11,
                METAL
            };

            enum class DrawMode
            {
                POINT_LIST = 0,
                LINE_LIST,
                LINE_STRIP,
                TRIANGLE_LIST,
                TRIANGLE_STRIP
            };

            virtual ~Renderer();

            Driver getDriver() const { return driver; }

            virtual void setClearColor(Color color) { clearColor = color; }
            virtual Color getClearColor() const { return clearColor; }

            virtual void clear();
            virtual void present();
            virtual void flush();

            const Size2& getSize() const { return size; }
            uint32_t getSampleCount() const { return sampleCount; }

            virtual std::vector<Size2> getSupportedResolutions() const;

            virtual BlendState* createBlendState(bool enableBlending,
                                                   BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                   BlendState::BlendOperation colorOperation,
                                                   BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                   BlendState::BlendOperation alphaOperation);
            virtual bool activateBlendState(BlendState* blendState);

            virtual Texture* createTexture(const Size2& textureSize, bool dynamic, bool mipmaps = true);
            virtual Texture* loadTextureFromFile(const std::string& filename, bool dynamic = false, bool mipmaps = true);
            virtual Texture* loadTextureFromData(const void* data, const Size2& textureSize, bool dynamic = false, bool mipmaps = true);
            virtual bool activateTexture(Texture* texture, uint32_t layer);
            virtual Texture* getActiveTexture(uint32_t layer) const { return activeTextures[layer]; }
            virtual RenderTarget* createRenderTarget(const Size2& renderTargetSize, bool depthBuffer);
            virtual bool activateRenderTarget(RenderTarget* renderTarget);

            virtual Shader* loadShaderFromFiles(const std::string& pixelShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "");
            virtual Shader* loadShaderFromBuffers(const uint8_t* pixelShader,
                                                    uint32_t pixelShaderSize,
                                                    const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "");
            virtual bool activateShader(Shader* shader);
            virtual Shader* getActiveShader() const { return activeShader; }

            virtual MeshBuffer* createMeshBuffer();
            virtual MeshBuffer* createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer);
            virtual bool drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0);

            Vector2 viewToScreenLocation(const Vector2& position);
            Vector2 screenToViewLocation(const Vector2& position);
            bool checkVisibility(const Matrix4& transform, const AABB2& boundingBox, scene::Camera* camera);

            virtual bool saveScreenshot(const std::string& filename);

            virtual uint32_t getDrawCallCount() const { return drawCallCount; }

        protected:
            Renderer(Driver pDriver = Driver::NONE);
            virtual bool init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount);

            virtual void setSize(const Size2& newSize);
            virtual void setFullscreen(bool newFullscreen);

            Driver driver;
            Size2 size;
            bool fullscreen = false;
            uint32_t sampleCount = 1; // MSAA sample count

            Color clearColor;

            BlendState* activeBlendState = nullptr;
            Texture* activeTextures[TEXTURE_LAYERS];
            Shader* activeShader = nullptr;
            RenderTarget* activeRenderTarget = nullptr;

            uint32_t drawCallCount = 0;
        };
    } // namespace graphics
} // namespace ouzel
