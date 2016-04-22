// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <d3d11.h>
#include "Renderer.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererD3D11: public Renderer
        {
            friend Engine;
        public:
            virtual ~RendererD3D11();

            virtual void clear() override;
            virtual void present() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

            virtual void setSize(const Size2& size) override;

            virtual void setFullscreen(bool fullscreen) override;

            virtual BlendState* createBlendState(bool enableBlending,
                                                   BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                   BlendState::BlendOperation colorOperation,
                                                   BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                   BlendState::BlendOperation alphaOperation) override;
            virtual bool activateBlendState(BlendState* blendState) override;

            virtual Texture* createTexture(const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual Texture* loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
            virtual Texture* loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;
            virtual bool activateTexture(Texture* texture, uint32_t layer) override;
            virtual RenderTarget* createRenderTarget(const Size2& size, bool depthBuffer) override;
            virtual bool activateRenderTarget(RenderTarget* renderTarget) override;

            virtual Shader* loadShaderFromFiles(const std::string& pixelShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "") override;
            virtual Shader* loadShaderFromBuffers(const uint8_t* pixelShader,
                                                    uint32_t pixelShaderSize, const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;
            virtual bool activateShader(Shader* shader) override;

            virtual MeshBuffer* createMeshBuffer();
            virtual MeshBuffer* createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer);
            virtual bool drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0);

            virtual bool saveScreenshot(const std::string& filename) override;

            ID3D11Device* getDevice() const { return _device; }
            ID3D11DeviceContext* getContext() const { return _context; }

        protected:
            RendererD3D11();

            void destroy();
            virtual bool init(const Size2& size, bool fullscreen) override;

            IDXGIOutput* getOutput() const;

        private:
            ID3D11Device* _device = nullptr;
            ID3D11DeviceContext* _context = nullptr;
            IDXGISwapChain* _swapChain = nullptr;
            IDXGIAdapter* _adapter = nullptr;
            ID3D11Texture2D* _backBuffer = nullptr;
            ID3D11RenderTargetView* _rtView = nullptr;
            ID3D11SamplerState* _samplerState = nullptr;
            ID3D11RasterizerState* _rasterizerState = nullptr;

            ID3D11DepthStencilState* _depthStencilState = nullptr;

            ID3D11ShaderResourceView* _resourceViews[TEXTURE_LAYERS];
            ID3D11SamplerState* _samplerStates[TEXTURE_LAYERS];
        };
    } // namespace graphics
} // namespace ouzel
