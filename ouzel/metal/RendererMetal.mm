// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "RenderTargetMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#include "BlendStateMetal.h"
#if OUZEL_PLATFORM_MACOS
    #include "macos/WindowMacOS.h"
    #include "ColorPSMacOS.h"
    #include "ColorVSMacOS.h"
    #include "TexturePSMacOS.h"
    #include "TextureVSMacOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSMacOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSMacOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSMacOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSMacOS_metallib
#elif OUZEL_PLATFORM_TVOS
    #include "tvos/WindowTVOS.h"
    #include "ColorPSTVOS.h"
    #include "ColorVSTVOS.h"
    #include "TexturePSTVOS.h"
    #include "TextureVSTVOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSTVOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSTVOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSTVOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSTVOS_metallib
#elif OUZEL_PLATFORM_IOS
    #include "ios/WindowIOS.h"
    #include "ColorPSIOS.h"
    #include "ColorVSIOS.h"
    #include "TexturePSIOS.h"
    #include "TextureVSIOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSIOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSIOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSIOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSIOS_metallib
#endif
#include "core/Engine.h"
#include "core/Cache.h"
#include "utils/Utils.h"
#include "stb_image_write.h"

namespace ouzel
{
    namespace graphics
    {
        bool RendererMetal::available()
        {
            id<MTLDevice> device = MTLCreateSystemDefaultDevice();

            if (device)
            {
                [device release];
                return true;
            }

            return false;
        }

        RendererMetal::RendererMetal():
            Renderer(Driver::METAL), dirty(false)
        {
            apiVersion = 1;
        }

        RendererMetal::~RendererMetal()
        {
            if (msaaTexture)
            {
                [msaaTexture release];
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder release];
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer release];
            }

            for (const auto& pipelineState : pipelineStates)
            {
                [pipelineState.second release];
            }

            if (commandQueue)
            {
                [commandQueue release];
            }

            if (samplerState)
            {
                [samplerState release];
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
            }

            if (device)
            {
                [device release];
            }
        }

        void RendererMetal::free()
        {
            Renderer::free();

            if (msaaTexture)
            {
                [msaaTexture release];
                msaaTexture = Nil;
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder release];
                currentRenderCommandEncoder = Nil;
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer release];
                currentCommandBuffer = Nil;
            }

            for (const auto& pipelineState : pipelineStates)
            {
                [pipelineState.second release];
            }

            pipelineStates.clear();

            if (commandQueue)
            {
                [commandQueue release];
                commandQueue = Nil;
            }

            if (samplerState)
            {
                [samplerState release];
                samplerState = Nil;
            }

            if (renderPassDescriptor)
            {
                [renderPassDescriptor release];
                renderPassDescriptor = Nil;
            }

            if (device)
            {
                [device release];
                device = Nil;
            }
        }

        bool RendererMetal::init(const WindowPtr& window,
                                 uint32_t newSampleCount,
                                 TextureFiltering newTextureFiltering,
                                 bool newVerticalSync)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!Renderer::init(window, newSampleCount, newTextureFiltering, newVerticalSync))
            {
                return false;
            }

            free();

            inflightSemaphore = dispatch_semaphore_create(3); // allow encoding up to 3 command buffers simultaneously

            device = MTLCreateSystemDefaultDevice();

            if (!device)
            {
                log(LOG_LEVEL_ERROR, "Failed to create Metal device");
                return false;
            }

            if (device.name)
            {
                log(LOG_LEVEL_INFO, "Using %s for rendering", [device.name cStringUsingEncoding:NSUTF8StringEncoding]);
            }

#if OUZEL_PLATFORM_MACOS
            view = (MTKViewPtr)std::static_pointer_cast<WindowMacOS>(window)->getNativeView();
#elif OUZEL_PLATFORM_TVOS
            view = (MTKViewPtr)std::static_pointer_cast<WindowTVOS>(window)->getNativeView();
#elif OUZEL_PLATFORM_IOS
            view = (MTKViewPtr)std::static_pointer_cast<WindowIOS>(window)->getNativeView();
#endif
            view.device = device;
            view.sampleCount = sampleCount;
            view.framebufferOnly = NO; // for screenshot capturing
            //_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
            {
                log(LOG_LEVEL_ERROR, "Failed to create Metal render pass descriptor");
                return false;
            }

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

            commandQueue = [device newCommandQueue];

            if (!commandQueue)
            {
                log(LOG_LEVEL_ERROR, "Failed to create Metal command queue");
                return false;
            }

            MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
            switch (textureFiltering)
            {
                case TextureFiltering::NONE:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureFiltering::LINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
                    break;
                case TextureFiltering::BILINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                    break;
                case TextureFiltering::TRILINEAR:
                    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
                    samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
                    break;
            }
            samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
            samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;

            samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
            [samplerDescriptor release];

            if (!samplerState)
            {
                log(LOG_LEVEL_ERROR, "Failed to create Metal sampler state");
                return false;
            }

            ShaderPtr textureShader = createShader();
            textureShader->initFromBuffers(std::vector<uint8_t>(std::begin(TEXTURE_PIXEL_SHADER_METAL), std::end(TEXTURE_PIXEL_SHADER_METAL)),
                                           std::vector<uint8_t>(std::begin(TEXTURE_VERTEX_SHADER_METAL), std::end(TEXTURE_VERTEX_SHADER_METAL)),
                                           VertexPCT::ATTRIBUTES,
                                           {{"color", 4 * sizeof(float)}},
                                           {{"modelViewProj", sizeof(Matrix4)}},
                                           256, 256,
                                           "main_ps", "main_vs");

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            ShaderPtr colorShader = createShader();
            colorShader->initFromBuffers(std::vector<uint8_t>(std::begin(COLOR_PIXEL_SHADER_METAL), std::end(COLOR_PIXEL_SHADER_METAL)),
                                         std::vector<uint8_t>(std::begin(COLOR_VERTEX_SHADER_METAL), std::end(COLOR_VERTEX_SHADER_METAL)),
                                         VertexPC::ATTRIBUTES,
                                         {{"color", 4 * sizeof(float)}},
                                         {{"modelViewProj", sizeof(Matrix4)}},
                                         256, 256,
                                         "main_ps", "main_vs");

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendStatePtr noBlendState = createBlendState();

            noBlendState->init(false,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD,
                               BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                               BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendStatePtr addBlendState = createBlendState();

            addBlendState->init(true,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD,
                                BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendStatePtr multiplyBlendState = createBlendState();

            multiplyBlendState->init(true,
                                     BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                     BlendState::BlendOperation::ADD,
                                     BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                     BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendStatePtr alphaBlendState = createBlendState();

            alphaBlendState->init(true,
                                  BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                  BlendState::BlendOperation::ADD,
                                  BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                  BlendState::BlendOperation::ADD);

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            TexturePtr whitePixelTexture = createTexture();
            whitePixelTexture->initFromBuffer( { 255, 255, 255, 255 }, Size2(1.0f, 1.0f), false, false);
            sharedEngine->getCache()->setTexture(TEXTURE_WHITE_PIXEL, whitePixelTexture);

            ready = true;

            return true;
        }

        bool RendererMetal::update()
        {
            if (dirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());

                dirty = false;
            }

            return true;
        }

        void RendererMetal::setClearColor(Color newColor)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setClearColor(newColor);

            dirty = true;
        }

        void RendererMetal::setSize(const Size2& newSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            Renderer::setSize(newSize);

            dirty = true;
        }

        bool RendererMetal::present()
        {
            if (!Renderer::present())
            {
                return false;
            }

            NSUInteger frameBufferWidth = view.currentDrawable.texture.width;
            NSUInteger frameBufferHeight = view.currentDrawable.texture.height;

            if (sampleCount > 1)
            {
                if (!msaaTexture ||
                    frameBufferWidth != msaaTexture.width ||
                    frameBufferHeight != msaaTexture.height)
                {
                    if (msaaTexture)
                    {
                        [msaaTexture release];
                        msaaTexture = Nil;
                    }

                    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                                    width: frameBufferWidth
                                                                                                   height: frameBufferHeight
                                                                                                mipmapped: NO];
                    desc.textureType = MTLTextureType2DMultisample;
                    desc.storageMode = MTLStorageModePrivate;
                    desc.sampleCount = sampleCount;
                    desc.usage = MTLTextureUsageRenderTarget;

                    msaaTexture = [device newTextureWithDescriptor: desc];

                    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
                    renderPassDescriptor.colorAttachments[0].texture = msaaTexture;
                }

                renderPassDescriptor.colorAttachments[0].resolveTexture = view.currentDrawable.texture;
            }
            else
            {
                renderPassDescriptor.colorAttachments[0].texture = view.currentDrawable.texture;
            }

            dispatch_semaphore_wait(inflightSemaphore, DISPATCH_TIME_FOREVER);

            currentCommandBuffer = [[commandQueue commandBuffer] retain];

            if (!currentCommandBuffer)
            {
                log(LOG_LEVEL_ERROR, "Failed to create Metal command buffer");
                return false;
            }

            __block dispatch_semaphore_t blockSemaphore = inflightSemaphore;
            [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer>)
             {
                 dispatch_semaphore_signal(blockSemaphore);
             }];

            MTLScissorRect scissorRect;

            if (!update())
            {
                return false;
            }

            std::vector<float> shaderData;

            if (drawQueue.empty())
            {
                frameBufferClearedFrame = currentFrame;

                if (clear)
                {
                    if (!createRenderCommandEncoder(renderPassDescriptor))
                    {
                        return false;
                    }

                    currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                }
            }
            else for (const DrawCommand& drawCommand : drawQueue)
            {
                MTLRenderPassDescriptorPtr newRenderPassDescriptor = Nil;
                bool clearBuffer = false;

                // render target
                if (drawCommand.renderTarget)
                {
                    std::shared_ptr<RenderTargetMetal> renderTargetMetal = std::static_pointer_cast<RenderTargetMetal>(drawCommand.renderTarget);

                    if (!renderTargetMetal->isReady())
                    {
                        // don't render if invalid render target
                        continue;
                    }

                    newRenderPassDescriptor = renderTargetMetal->getRenderPassDescriptor();

                    std::shared_ptr<TextureMetal> renderTargetTextureMetal = std::static_pointer_cast<TextureMetal>(renderTargetMetal->getTexture());

                    scissorRect.x = scissorRect.y = 0;
                    scissorRect.width = renderTargetTextureMetal->getTexture().width;
                    scissorRect.height = renderTargetTextureMetal->getTexture().height;

                    if (renderTargetMetal->getFrameBufferClearedFrame() != currentFrame)
                    {
                        renderTargetMetal->setFrameBufferClearedFrame(currentFrame);
                        clearBuffer = renderTargetMetal->getClear();
                    }
                }
                else
                {
                    newRenderPassDescriptor = renderPassDescriptor;

                    scissorRect.x = scissorRect.y = 0;
                    scissorRect.width = frameBufferWidth;
                    scissorRect.height = frameBufferHeight;

                    if (frameBufferClearedFrame != currentFrame)
                    {
                        frameBufferClearedFrame = currentFrame;
                        clearBuffer = clear;
                    }
                }

                if (currentRenderPassDescriptor != newRenderPassDescriptor ||
                    !currentRenderCommandEncoder)
                {
                    if (!createRenderCommandEncoder(newRenderPassDescriptor))
                    {
                        return false;
                    }

                    currentRenderPassDescriptor.colorAttachments[0].loadAction = clearBuffer ? MTLLoadActionClear : MTLLoadActionLoad;
                }

                [currentRenderCommandEncoder setTriangleFillMode:drawCommand.wireframe ? MTLTriangleFillModeLines : MTLTriangleFillModeFill];

                // scissor test
                if (drawCommand.scissorTestEnabled)
                {
                    scissorRect.x = static_cast<NSUInteger>(drawCommand.scissorTest.x);
                    scissorRect.y = static_cast<NSUInteger>(drawCommand.scissorTest.y);
                    scissorRect.width = static_cast<NSUInteger>(drawCommand.scissorTest.width);
                    scissorRect.height = static_cast<NSUInteger>(drawCommand.scissorTest.height);
                }

                [currentRenderCommandEncoder setScissorRect: scissorRect];

                // shader
                std::shared_ptr<ShaderMetal> shaderMetal = std::static_pointer_cast<ShaderMetal>(drawCommand.shader);

                if (!shaderMetal || !shaderMetal->isReady())
                {
                    // don't render if invalid shader
                    continue;
                }

                // pixel shader constants
                const std::vector<ShaderMetal::Location>& pixelShaderConstantLocations = shaderMetal->getPixelShaderConstantLocations();

                if (drawCommand.pixelShaderConstants.size() > pixelShaderConstantLocations.size())
                {
                    log(LOG_LEVEL_ERROR, "Invalid pixel shader constant size");
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.pixelShaderConstants.size(); ++i)
                {
                    const ShaderMetal::Location& pixelShaderConstantLocation = pixelShaderConstantLocations[i];
                    const std::vector<float>& pixelShaderConstant = drawCommand.pixelShaderConstants[i];

                    if (vectorDataSize(pixelShaderConstant) != pixelShaderConstantLocation.size)
                    {
                        log(LOG_LEVEL_ERROR, "Invalid pixel shader constant size");
                        return false;
                    }

                    shaderData.insert(shaderData.end(), pixelShaderConstant.begin(), pixelShaderConstant.end());
                }

                shaderMetal->uploadBuffer(shaderMetal->getPixelShaderConstantBuffer(),
                                          shaderMetal->getPixelShaderConstantBufferOffset(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(vectorDataSize(shaderData)));

                [currentRenderCommandEncoder setFragmentBuffer:shaderMetal->getPixelShaderConstantBuffer()
                                                        offset:shaderMetal->getPixelShaderConstantBufferOffset()
                                                       atIndex:1];


                // vertex shader constants
                const std::vector<ShaderMetal::Location>& vertexShaderConstantLocations = shaderMetal->getVertexShaderConstantLocations();

                if (drawCommand.vertexShaderConstants.size() > vertexShaderConstantLocations.size())
                {
                    log(LOG_LEVEL_ERROR, "Invalid vertex shader constant size");
                    return false;
                }

                shaderData.clear();

                for (size_t i = 0; i < drawCommand.vertexShaderConstants.size(); ++i)
                {
                    const ShaderMetal::Location& vertexShaderConstantLocation = vertexShaderConstantLocations[i];
                    const std::vector<float>& vertexShaderConstant = drawCommand.vertexShaderConstants[i];

                    if (vectorDataSize(vertexShaderConstant) != vertexShaderConstantLocation.size)
                    {
                        log(LOG_LEVEL_ERROR, "Invalid vertex shader constant size");
                        return false;
                    }

                    shaderData.insert(shaderData.end(), vertexShaderConstant.begin(), vertexShaderConstant.end());
                }

                shaderMetal->uploadBuffer(shaderMetal->getVertexShaderConstantBuffer(),
                                          shaderMetal->getVertexShaderConstantBufferOffset(),
                                          shaderData.data(),
                                          static_cast<uint32_t>(vectorDataSize(shaderData)));

                [currentRenderCommandEncoder setVertexBuffer:shaderMetal->getVertexShaderConstantBuffer()
                                                      offset:shaderMetal->getVertexShaderConstantBufferOffset()
                                                     atIndex:1];

                shaderMetal->nextBuffers();

                // blend state
                std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(drawCommand.blendState);

                if (!blendStateMetal || !blendStateMetal->isReady())
                {
                    // don't render if invalid blend state
                    continue;
                }

                auto pipelineStateIterator = pipelineStates.find(std::make_pair(blendStateMetal, shaderMetal));

                if (pipelineStateIterator != pipelineStates.end())
                {
                    [currentRenderCommandEncoder setRenderPipelineState:pipelineStateIterator->second];
                }
                else
                {
                    id<MTLRenderPipelineState> pipelineState = createPipelineState(blendStateMetal, shaderMetal);

                    if (!pipelineState)
                    {
                        return false;
                    }

                    [currentRenderCommandEncoder setRenderPipelineState:pipelineState];
                }

                // textures
                for (uint32_t layer = 0; layer < Texture::LAYERS; ++layer)
                {
                    std::shared_ptr<TextureMetal> textureMetal;

                    if (drawCommand.textures.size() > layer)
                    {
                        textureMetal = std::static_pointer_cast<TextureMetal>(drawCommand.textures[layer]);
                    }

                    if (textureMetal)
                    {
                        if (!textureMetal->isReady())
                        {
                            // don't render if invalid texture
                            continue;
                        }

                        [currentRenderCommandEncoder setFragmentTexture:textureMetal->getTexture() atIndex:layer];
                        [currentRenderCommandEncoder setFragmentSamplerState:samplerState atIndex:layer];
                    }
                    else
                    {
                        [currentRenderCommandEncoder setFragmentTexture:Nil atIndex:layer];
                    }
                }

                // mesh buffer
                std::shared_ptr<MeshBufferMetal> meshBufferMetal = std::static_pointer_cast<MeshBufferMetal>(drawCommand.meshBuffer);

                if (!meshBufferMetal || !meshBufferMetal->isReady())
                {
                    // don't render if invalid mesh buffer
                    continue;
                }

                [currentRenderCommandEncoder setVertexBuffer:meshBufferMetal->getVertexBuffer() offset:0 atIndex:0];

                // draw
                MTLPrimitiveType primitiveType;

                switch (drawCommand.drawMode)
                {
                    case DrawMode::POINT_LIST: primitiveType = MTLPrimitiveTypePoint; break;
                    case DrawMode::LINE_LIST: primitiveType = MTLPrimitiveTypeLine; break;
                    case DrawMode::LINE_STRIP: primitiveType = MTLPrimitiveTypeLineStrip; break;
                    case DrawMode::TRIANGLE_LIST: primitiveType = MTLPrimitiveTypeTriangle; break;
                    case DrawMode::TRIANGLE_STRIP: primitiveType = MTLPrimitiveTypeTriangleStrip; break;
                    default: log(LOG_LEVEL_ERROR, "Invalid draw mode"); return false;
                }

                [currentRenderCommandEncoder drawIndexedPrimitives:primitiveType
                                                        indexCount:drawCommand.indexCount
                                                         indexType:meshBufferMetal->getIndexFormat()
                                                       indexBuffer:meshBufferMetal->getIndexBuffer()
                                                 indexBufferOffset:drawCommand.startIndex * meshBufferMetal->getBytesPerIndex()];
            }

            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder endEncoding];
                [currentRenderCommandEncoder release];
                currentRenderCommandEncoder = Nil;
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer presentDrawable:view.currentDrawable];

                [currentCommandBuffer commit];
                [currentCommandBuffer release];
                currentCommandBuffer = Nil;
            }

            if (!saveScreenshots())
            {
                return false;
            }

            return true;
        }

        std::vector<Size2> RendererMetal::getSupportedResolutions() const
        {
            return std::vector<Size2>();
        }

        BlendStatePtr RendererMetal::createBlendState()
        {
            std::shared_ptr<BlendStateMetal> blendState(new BlendStateMetal());
            return blendState;
        }

        TexturePtr RendererMetal::createTexture()
        {
            std::shared_ptr<TextureMetal> texture(new TextureMetal());
            return texture;
        }

        RenderTargetPtr RendererMetal::createRenderTarget()
        {
            std::shared_ptr<RenderTargetMetal> renderTarget(new RenderTargetMetal());
            return renderTarget;
        }

        ShaderPtr RendererMetal::createShader()
        {
            std::shared_ptr<ShaderMetal> shader(new ShaderMetal());
            return shader;
        }

        MeshBufferPtr RendererMetal::createMeshBuffer()
        {
            std::shared_ptr<MeshBufferMetal> meshBuffer(new MeshBufferMetal());
            return meshBuffer;
        }

        MTLRenderPipelineStatePtr RendererMetal::createPipelineState(const std::shared_ptr<BlendStateMetal>& blendState,
                                                                     const std::shared_ptr<ShaderMetal>& shader)
        {
            MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
            pipelineStateDescriptor.sampleCount = view.sampleCount;
            pipelineStateDescriptor.vertexFunction = shader->getVertexShader();
            pipelineStateDescriptor.fragmentFunction = shader->getPixelShader();
            pipelineStateDescriptor.vertexDescriptor = shader->getVertexDescriptor();
            pipelineStateDescriptor.depthAttachmentPixelFormat = view.depthStencilPixelFormat;
            pipelineStateDescriptor.stencilAttachmentPixelFormat = view.depthStencilPixelFormat;

            pipelineStateDescriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat;

            // blending
            std::shared_ptr<BlendStateMetal> blendStateMetal = std::static_pointer_cast<BlendStateMetal>(blendState);

            pipelineStateDescriptor.colorAttachments[0].blendingEnabled = blendState->isBlendingEnabled() ? YES : NO;

            pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = blendStateMetal->getSourceRGBBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = blendStateMetal->getDestinationRGBBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = blendStateMetal->getRGBBlendOperation();

            pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = blendStateMetal->getSourceAlphaBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = blendStateMetal->getDestinationAlphaBlendFactor();
            pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = blendStateMetal->getAlphaBlendOperation();

            pipelineStateDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;

            NSError* error = Nil;
            id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            [pipelineStateDescriptor release];
            if (error || !pipelineState)
            {
                if (pipelineState) [pipelineState release];

                log(LOG_LEVEL_ERROR, "Failed to created Metal pipeline state");
                return Nil;
            }

            pipelineStates[std::make_pair(blendState, shader)] = pipelineState;

            return pipelineState;
        }

        bool RendererMetal::saveScreenshots()
        {
            for (;;)
            {
                std::string filename;

                {
                    std::lock_guard<std::mutex> lock(screenshotMutex);

                    if (screenshotQueue.empty()) break;

                    filename = screenshotQueue.front();
                    screenshotQueue.pop();
                }

                MTLTexturePtr texture = view.currentDrawable.texture;

                if (!texture)
                {
                    return false;
                }

                NSUInteger width = static_cast<NSUInteger>(texture.width);
                NSUInteger height = static_cast<NSUInteger>(texture.height);

                std::shared_ptr<uint8_t> data(new uint8_t[width * height * 4]);
                [texture getBytes:data.get() bytesPerRow:width * 4 fromRegion:MTLRegionMake2D(0, 0, width, height) mipmapLevel:0];

                uint8_t temp;
                for (uint32_t y = 0; y < height; ++y)
                {
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        temp = data.get()[((y * width + x) * 4)];
                        data.get()[((y * width + x) * 4)] = data.get()[((y * width + x) * 4) + 2];
                        data.get()[((y * width + x) * 4) + 2] = temp;
                        data.get()[((y * width + x) * 4) + 3] = 255;
                    }
                }

                if (!stbi_write_png(filename.c_str(), static_cast<int>(width), static_cast<int>(height), 4, data.get(), static_cast<int>(width * 4)))
                {
                    log(LOG_LEVEL_ERROR, "Failed to save image to file");
                    return false;
                }
            }

            return true;
        }

        bool RendererMetal::createRenderCommandEncoder(MTLRenderPassDescriptorPtr newRenderPassDescriptor)
        {
            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder endEncoding];
                [currentRenderCommandEncoder release];
            }

            currentRenderPassDescriptor = newRenderPassDescriptor;
            currentRenderCommandEncoder = [[currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor] retain];

            if (!currentRenderCommandEncoder)
            {
                log(LOG_LEVEL_ERROR, "Failed to create Metal render command encoder");
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
