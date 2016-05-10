// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"
#include "RendererMetal.h"
#include "TextureMetal.h"
#include "RenderTargetMetal.h"
#include "ShaderMetal.h"
#include "MeshBufferMetal.h"
#include "BlendStateMetal.h"
#ifdef OUZEL_PLATFORM_OSX
    #include "WindowOSX.h"
    #include "ColorPSOSX.h"
    #include "ColorVSOSX.h"
    #include "TexturePSOSX.h"
    #include "TextureVSOSX.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSOSX_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSOSX_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSOSX_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSOSX_metallib
#elif OUZEL_PLATFORM_TVOS
    #include "WindowTVOS.h"
    #include "ColorPSTVOS.h"
    #include "ColorVSTVOS.h"
    #include "TexturePSTVOS.h"
    #include "TextureVSTVOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSTVOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSTVOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSTVOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSTVOS_metallib
#elif OUZEL_PLATFORM_IOS
    #include "WindowIOS.h"
    #include "ColorPSIOS.h"
    #include "ColorVSIOS.h"
    #include "TexturePSIOS.h"
    #include "TextureVSIOS.h"
    #define COLOR_PIXEL_SHADER_METAL ColorPSIOS_metallib
    #define COLOR_VERTEX_SHADER_METAL ColorVSIOS_metallib
    #define TEXTURE_PIXEL_SHADER_METAL TexturePSIOS_metallib
    #define TEXTURE_VERTEX_SHADER_METAL TextureVSIOS_metallib
#endif
#include "Engine.h"
#include "Cache.h"
#include "Utils.h"

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
            Renderer(Driver::METAL)
        {

        }

        RendererMetal::~RendererMetal()
        {
            destroy();
        }

        void RendererMetal::destroy()
        {
            if (msaaTexture)
            {
                [msaaTexture release];
                msaaTexture = Nil;
            }
            
            if (currentRenderCommandEncoder)
            {
                [currentRenderCommandEncoder release];
            }

            if (currentCommandBuffer)
            {
                [currentCommandBuffer release];
                currentCommandBuffer = Nil;
            }

            for (auto pipelineState : pipelineStates)
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

        bool RendererMetal::init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount)
        {
            if (!Renderer::init(newSize, newFullscreen, newSampleCount))
            {
                return false;
            }

            destroy();

            inflightSemaphore = dispatch_semaphore_create(3); // allow encoding up to 3 command buffers simultaneously

            device = MTLCreateSystemDefaultDevice();

            if (!device)
            {
                log("Failed to create Metal device");
                return false;
            }
#ifdef OUZEL_PLATFORM_OSX
            WindowOSX* window = static_cast<WindowOSX*>(sharedEngine->getWindow());
#elif OUZEL_PLATFORM_TVOS
            WindowTVOS* window = static_cast<WindowTVOS*>(sharedEngine->getWindow());
#elif OUZEL_PLATFORM_IOS
            WindowIOS* window = static_cast<WindowIOS*>(sharedEngine->getWindow());
#endif
            view = static_cast<MTKView*>(window->getNativeView());
            view.device = device;
            view.sampleCount = sampleCount;
            //_view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

            renderPassDescriptor = [[MTLRenderPassDescriptor renderPassDescriptor] retain];

            if (!renderPassDescriptor)
            {
                log("Failed to create Metal render pass descriptor");
                return false;
            }

            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

            commandQueue = [device newCommandQueue];

            if (!commandQueue)
            {
                log("Failed to create Metal command queue");
                return false;
            }

            MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
            samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
            samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
            samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
            samplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
            samplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;

            samplerState = [device newSamplerStateWithDescriptor:samplerDescriptor];
            [samplerDescriptor release];

            if (!samplerState)
            {
                log("Failed to create Metal sampler state");
                return false;
            }

            Shader* textureShader = loadShaderFromBuffers(TEXTURE_PIXEL_SHADER_METAL, sizeof(TEXTURE_PIXEL_SHADER_METAL), TEXTURE_VERTEX_SHADER_METAL, sizeof(TEXTURE_VERTEX_SHADER_METAL), VertexPCT::ATTRIBUTES, "main_ps", "main_vs");

            if (!textureShader)
            {
                return false;
            }

            textureShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}}, 256);
            textureShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_TEXTURE, textureShader);

            Shader* colorShader = loadShaderFromBuffers(COLOR_PIXEL_SHADER_METAL, sizeof(COLOR_PIXEL_SHADER_METAL), COLOR_VERTEX_SHADER_METAL, sizeof(COLOR_VERTEX_SHADER_METAL), VertexPC::ATTRIBUTES, "main_ps", "main_vs");

            if (!colorShader)
            {
                return false;
            }

            colorShader->setVertexShaderConstantInfo({{"modelViewProj", sizeof(Matrix4)}}, 256);
            colorShader->setPixelShaderConstantInfo({{"color", 4 * sizeof(float)}}, 256);

            sharedEngine->getCache()->setShader(SHADER_COLOR, colorShader);

            BlendState* noBlendState = createBlendState(false,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD,
                                                          BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                          BlendState::BlendOperation::ADD);

            if (!noBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_NO_BLEND, noBlendState);

            BlendState* addBlendState = createBlendState(true,
                                                           BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                                           BlendState::BlendOperation::ADD,
                                                           BlendState::BlendFactor::ONE, BlendState::BlendFactor::ONE,
                                                           BlendState::BlendOperation::ADD);

            if (!addBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ADD, addBlendState);

            BlendState* multiplyBlendState = createBlendState(true,
                                                                BlendState::BlendFactor::DEST_COLOR, BlendState::BlendFactor::ZERO,
                                                                BlendState::BlendOperation::ADD,
                                                                BlendState::BlendFactor::DEST_ALPHA, BlendState::BlendFactor::ZERO,
                                                                BlendState::BlendOperation::ADD);

            if (!multiplyBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_MULTIPLY, multiplyBlendState);

            BlendState* alphaBlendState = createBlendState(true,
                                                             BlendState::BlendFactor::SRC_ALPHA, BlendState::BlendFactor::INV_SRC_ALPHA,
                                                             BlendState::BlendOperation::ADD,
                                                             BlendState::BlendFactor::ONE, BlendState::BlendFactor::ZERO,
                                                             BlendState::BlendOperation::ADD);

            if (!alphaBlendState)
            {
                return false;
            }

            sharedEngine->getCache()->setBlendState(BLEND_ALPHA, alphaBlendState);

            for (Shader* shader : {textureShader, colorShader})
            {
                ShaderMetal* shaderMetal = static_cast<ShaderMetal*>(shader);

                for (BlendState* blendState : {noBlendState, alphaBlendState})
                {
                    BlendStateMetal* blendStateMetal = static_cast<BlendStateMetal*>(blendState);

                    if (!createPipelineState(blendStateMetal, shaderMetal))
                    {
                        return false;
                    }
                }

                if (!createPipelineState(nullptr, shaderMetal))
                {
                    return false;
                }
            }

            setSize(size);

            return true;
        }

        void RendererMetal::setClearColor(Color newColor)
        {
            Renderer::setClearColor(newColor);

            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clearColor.getR(), clearColor.getG(), clearColor.getB(), clearColor.getA());
        }

        void RendererMetal::setSize(const Size2& newSize)
        {
            Renderer::setSize(newSize);
        }

        void RendererMetal::clear()
        {
            clearedRenderPassDescriptors.clear();
            
            if (sampleCount > 1)
            {
                if (!msaaTexture ||
                    view.currentDrawable.texture.width != msaaTexture.width ||
                    view.currentDrawable.texture.height != msaaTexture.height)
                {
                    if (msaaTexture)
                    {
                        [msaaTexture release];
                        msaaTexture = Nil;
                    }

                    MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                                                                                    width: view.currentDrawable.texture.width
                                                                                                   height: view.currentDrawable.texture.height
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
                log("Failed to create Metal command buffer");
                return;
            }

            __block dispatch_semaphore_t blockSemaphore = inflightSemaphore;
            [currentCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
             {
                 OUZEL_UNUSED(buffer);
                 dispatch_semaphore_signal(blockSemaphore);
             }];
        }

        void RendererMetal::present()
        {
            Renderer::present();

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
        }

        Texture* RendererMetal::loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps)
        {
            TextureMetal* texture = new TextureMetal();

            if (!texture->initFromFile(filename, dynamic, mipmaps))
            {
                texture->release();
                texture = nullptr;
            }

            return texture;
        }

        Texture* RendererMetal::loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps)
        {
            TextureMetal* texture = new TextureMetal();

            if (!texture->initFromData(data, size, dynamic, mipmaps))
            {
                texture->release();
                texture = nullptr;
            }

            return texture;
        }

        RenderTarget* RendererMetal::createRenderTarget(const Size2& size, bool depthBuffer)
        {
            RenderTargetMetal* renderTarget = new RenderTargetMetal();

            if (!renderTarget->init(size, depthBuffer))
            {
                renderTarget->release();
                renderTarget = nullptr;
            }

            return renderTarget;
        }

        bool RendererMetal::activateRenderTarget(RenderTarget* renderTarget)
        {
            if (!Renderer::activateRenderTarget(renderTarget))
            {
                return false;
            }

            MTLRenderPassDescriptorPtr newRenderPassDescriptor = Nil;

            if (activeRenderTarget)
            {
                RenderTargetMetal* renderTargetMetal = static_cast<RenderTargetMetal*>(activeRenderTarget);

                newRenderPassDescriptor = renderTargetMetal->getRenderPassDescriptor();
            }
            else
            {
                newRenderPassDescriptor = renderPassDescriptor;
            }

            if (currentRenderPassDescriptor != newRenderPassDescriptor || !currentRenderCommandEncoder)
            {
                if (currentRenderCommandEncoder)
                {
                    [currentRenderCommandEncoder endEncoding];
                    [currentRenderCommandEncoder release];
                }

                currentRenderPassDescriptor = newRenderPassDescriptor;

                if (clearedRenderPassDescriptors.find(currentRenderPassDescriptor) == clearedRenderPassDescriptors.end())
                {
                    currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    clearedRenderPassDescriptors.insert(currentRenderPassDescriptor);
                }
                else
                {
                    currentRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
                }

                currentRenderCommandEncoder = [[currentCommandBuffer renderCommandEncoderWithDescriptor:currentRenderPassDescriptor] retain];

                if (!currentRenderCommandEncoder)
                {
                    log("Failed to create Metal render command encoder");
                    return false;
                }
            }

            return true;
        }

        Shader* RendererMetal::loadShaderFromFiles(const std::string& pixelShader,
                                                     const std::string& vertexShader,
                                                     uint32_t vertexAttributes,
                                                     const std::string& pixelShaderFunction,
                                                     const std::string& vertexShaderFunction)
        {
            ShaderMetal* shader = new ShaderMetal();

            if (!shader->initFromFiles(pixelShader, vertexShader, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader->release();
                shader = nullptr;
            }

            return shader;
        }

        Shader* RendererMetal::loadShaderFromBuffers(const uint8_t* pixelShader,
                                                       uint32_t pixelShaderSize,
                                                       const uint8_t* vertexShader,
                                                       uint32_t vertexShaderSize,
                                                       uint32_t vertexAttributes,
                                                       const std::string& pixelShaderFunction,
                                                       const std::string& vertexShaderFunction)
        {
            ShaderMetal* shader = new ShaderMetal();

            if (!shader->initFromBuffers(pixelShader, pixelShaderSize, vertexShader, vertexShaderSize, vertexAttributes, pixelShaderFunction, vertexShaderFunction))
            {
                shader->release();
                shader = nullptr;
            }

            return shader;
        }

        MeshBuffer* RendererMetal::createMeshBuffer()
        {
            MeshBufferMetal* meshBuffer = new MeshBufferMetal();

            if (!meshBuffer->init())
            {
                meshBuffer->release();
                meshBuffer = nullptr;
            }

            return meshBuffer;
        }

        MeshBuffer* RendererMetal::createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            MeshBufferMetal* meshBuffer = new MeshBufferMetal();

            if (!meshBuffer->initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer))
            {
                meshBuffer->release();
                meshBuffer = nullptr;
            }

            return meshBuffer;
        }

        bool RendererMetal::drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount, DrawMode drawMode, uint32_t startIndex)
        {
            if (!Renderer::drawMeshBuffer(meshBuffer, indexCount, drawMode))
            {
                return false;
            }

            if (!activeShader)
            {
                return false;
            }

            if (!currentCommandBuffer)
            {
                log("No active Metal render command buffer");
                return false;
            }

            if (!currentRenderCommandEncoder)
            {
                log("No active Metal render command encoder");
                return false;
            }

            MeshBufferMetal* meshBufferMetal = static_cast<MeshBufferMetal*>(meshBuffer);

            [currentRenderCommandEncoder setVertexBuffer:meshBufferMetal->getVertexBuffer() offset:0 atIndex:0];

            ShaderMetal* shaderMetal = static_cast<ShaderMetal*>(activeShader);
            [currentRenderCommandEncoder setFragmentBuffer:shaderMetal->getPixelShaderConstantBuffer()
                                                    offset:shaderMetal->getPixelShaderConstantBufferOffset()
                                                   atIndex:1];
            [currentRenderCommandEncoder setVertexBuffer:shaderMetal->getVertexShaderConstantBuffer()
                                                  offset:shaderMetal->getVertexShaderConstantBufferOffset()
                                                 atIndex:1];

            BlendStateMetal* blendStateMetal = static_cast<BlendStateMetal*>(activeBlendState);

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

            for (uint32_t layer = 0; layer < TEXTURE_LAYERS; ++layer)
            {
                if (activeTextures[layer])
                {
                    TextureMetal* textureMetal = static_cast<TextureMetal*>(activeTextures[layer]);
                    
                    [currentRenderCommandEncoder setFragmentTexture:textureMetal->getTexture() atIndex:layer];
                }
                else
                {
                    [currentRenderCommandEncoder setFragmentTexture:Nil atIndex:layer];
                }
            }

            [currentRenderCommandEncoder setFragmentSamplerState:samplerState atIndex:0];

            if (indexCount == 0)
            {
                indexCount = meshBufferMetal->getIndexCount() - startIndex;
            }

            MTLPrimitiveType primitiveType;

            switch (drawMode)
            {
                case DrawMode::POINT_LIST: primitiveType = MTLPrimitiveTypePoint; break;
                case DrawMode::LINE_LIST: primitiveType = MTLPrimitiveTypeLine; break;
                case DrawMode::LINE_STRIP: primitiveType = MTLPrimitiveTypeLineStrip; break;
                case DrawMode::TRIANGLE_LIST: primitiveType = MTLPrimitiveTypeTriangle; break;
                case DrawMode::TRIANGLE_STRIP: primitiveType = MTLPrimitiveTypeTriangleStrip; break;
                default: return false;
            }

            [currentRenderCommandEncoder drawIndexedPrimitives:primitiveType
                                                     indexCount:indexCount
                                                      indexType:meshBufferMetal->getIndexFormat()
                                                    indexBuffer:meshBufferMetal->getIndexBuffer()
                                              indexBufferOffset:static_cast<NSUInteger>(startIndex * meshBuffer->getIndexSize())];

            shaderMetal->nextBuffers();

            return true;
        }

        MTLRenderPipelineStatePtr RendererMetal::createPipelineState(BlendStateMetal* blendState,
                                                                     ShaderMetal* shader)
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
            if (!blendState)
            {
                pipelineStateDescriptor.colorAttachments[0].blendingEnabled = NO;

                pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorZero;
                pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;

                pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorZero;
                pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
            }
            else
            {
                pipelineStateDescriptor.colorAttachments[0].blendingEnabled = blendState->isBlendingEnabled() ? YES : NO;

                pipelineStateDescriptor.colorAttachments[0].sourceRGBBlendFactor = BlendStateMetal::getBlendFactor(blendState->getColorBlendSource());
                pipelineStateDescriptor.colorAttachments[0].destinationRGBBlendFactor = BlendStateMetal::getBlendFactor(blendState->getColorBlendDest());
                pipelineStateDescriptor.colorAttachments[0].rgbBlendOperation = BlendStateMetal::getBlendOperation(blendState->getColorOperation());

                pipelineStateDescriptor.colorAttachments[0].sourceAlphaBlendFactor = BlendStateMetal::getBlendFactor(blendState->getAlphaBlendSource());
                pipelineStateDescriptor.colorAttachments[0].destinationAlphaBlendFactor = BlendStateMetal::getBlendFactor(blendState->getAlphaBlendDest());
                pipelineStateDescriptor.colorAttachments[0].alphaBlendOperation = BlendStateMetal::getBlendOperation(blendState->getAlphaOperation());
            }

            pipelineStateDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;

            NSError* error = Nil;
            id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
            [pipelineStateDescriptor release];
            if (error || !pipelineState)
            {
                if (pipelineState) [pipelineState release];
                
                log("Failed to created Metal pipeline state");
                return Nil;
            }

            pipelineStates[std::make_pair(blendState, shader)] = pipelineState;

            return pipelineState;
        }
    } // namespace graphics
} // namespace ouzel
