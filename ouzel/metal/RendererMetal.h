// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include <map>
#include <dispatch/dispatch.h>

#ifdef __OBJC__
#import <Metal/Metal.h>
#import <MetalKit/MTKView.h>
typedef MTKView* MTKViewPtr;
typedef id<MTLDevice> MTLDevicePtr;
typedef MTLRenderPassDescriptor* MTLRenderPassDescriptorPtr;
typedef id<MTLSamplerState> MTLSamplerStatePtr;
typedef id<MTLCommandQueue> MTLCommandQueuePtr;
typedef id<MTLRenderPipelineState> MTLRenderPipelineStatePtr;
typedef id<MTLCommandBuffer> MTLCommandBufferPtr;
typedef id<MTLRenderCommandEncoder> MTLRenderCommandEncoderPtr;
typedef id <MTLTexture> MTLTexturePtr;
#else
#include <objc/objc.h>
typedef id MTKViewPtr;
typedef id MTLDevicePtr;
typedef id MTLRenderPassDescriptorPtr;
typedef id MTLSamplerStatePtr;
typedef id MTLCommandQueuePtr;
typedef id MTLRenderPipelineStatePtr;
typedef id MTLCommandBufferPtr;
typedef id MTLRenderCommandEncoderPtr;
typedef id MTLTexturePtr;
#endif

#include "Renderer.h"

namespace ouzel
{
    class Engine;
    
    namespace graphics
    {
        class BlendStateMetal;
        class ShaderMetal;

        class RendererMetal: public Renderer
        {
            friend Engine;
        public:
            static bool available();

            virtual ~RendererMetal();

            virtual bool init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount) override;

            virtual void setClearColor(Color newColor) override;

            virtual void setSize(const Size2& newSize) override;

            virtual void clear() override;
            virtual void present() override;

            virtual Texture* loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
            virtual Texture* loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;

            virtual RenderTarget* createRenderTarget(const Size2& size, bool depthBuffer) override;
            virtual bool activateRenderTarget(RenderTarget* renderTarget) override;

            virtual Shader* loadShaderFromFiles(const std::string& pixelShader,
                                                  const std::string& vertexShader,
                                                  uint32_t vertexAttributes,
                                                  const std::string& pixelShaderFunction = "",
                                                  const std::string& vertexShaderFunction = "") override;
            virtual Shader* loadShaderFromBuffers(const uint8_t* pixelShader,
                                                    uint32_t pixelShaderSize,
                                                    const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;

            virtual MeshBuffer* createMeshBuffer() override;
            virtual MeshBuffer* createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;
            virtual bool drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0) override;

            MTLDevicePtr getDevice() const { return device; }
            MTKViewPtr getMetalView() const { return view; }

        protected:
            RendererMetal();

            void destroy();
            MTLRenderPipelineStatePtr createPipelineState(BlendStateMetal* blendState,
                                                          ShaderMetal* shader);

            MTKViewPtr view = Nil;

            MTLDevicePtr device = Nil;
            MTLRenderPassDescriptorPtr renderPassDescriptor = Nil;
            std::set<MTLRenderPassDescriptorPtr> clearedRenderPassDescriptors;

            MTLSamplerStatePtr samplerState = Nil;
            MTLCommandQueuePtr commandQueue = Nil;

            MTLTexturePtr msaaTexture = Nil;

            MTLCommandBufferPtr currentCommandBuffer = Nil;

            MTLRenderPassDescriptorPtr currentRenderPassDescriptor = Nil;
            MTLRenderCommandEncoderPtr currentRenderCommandEncoder = Nil;

            dispatch_semaphore_t inflightSemaphore;

            std::map<std::pair<BlendStateMetal*, ShaderMetal*>, MTLRenderPipelineStatePtr> pipelineStates;
        };
    } // namespace graphics
} // namespace ouzel
