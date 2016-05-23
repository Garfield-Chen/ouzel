// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <set>
#include "CompileConfig.h"
#include "Renderer.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif defined(OUZEL_PLATFORM_LINUX)
    #ifndef GL_GLEXT_PROTOTYPES
        #define GL_GLEXT_PROTOTYPES 1
    #endif
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#endif

#if defined(OUZEL_PLATFORM_ANDROID)
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;
#endif

namespace ouzel
{
    class Engine;
    
    namespace graphics
    {
        class RendererOGL: public Renderer
        {
            friend Engine;
        public:
            virtual ~RendererOGL();

            void setFrameBuffer(GLuint newFrameBuffer);

            static bool checkOpenGLErrors(bool logError = true);

            virtual void setClearColor(Color color) override;

            virtual void clear() override;
            virtual void present() override;
            virtual void flush() override;

            virtual std::vector<Size2> getSupportedResolutions() const override;

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
                                                    uint32_t pixelShaderSize,
                                                    const uint8_t* vertexShader,
                                                    uint32_t vertexShaderSize,
                                                    uint32_t vertexAttributes,
                                                    const std::string& pixelShaderFunction = "",
                                                    const std::string& vertexShaderFunction = "") override;
            virtual bool activateShader(Shader* shader) override;

            virtual MeshBuffer* createMeshBuffer() override;
            virtual MeshBuffer* createMeshBufferFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer) override;
            virtual bool drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST, uint32_t startIndex = 0) override;

            virtual bool saveScreenshot(const std::string& filename) override;

            static bool bindTexture(GLuint textureId, uint32_t layer);
            static bool bindProgram(GLuint programId);
            static bool bindFrameBuffer(GLuint frameBufferId);
            static bool bindElementArrayBuffer(GLuint elementArrayBufferId);
            static bool bindArrayBuffer(GLuint arrayBufferId);
            static bool bindVertexArray(GLuint vertexArrayId);

            static bool unbindTexture(GLuint textureId);
            static bool unbindProgram(GLuint programId);
            static bool unbindFrameBuffer(GLuint frameBufferId);
            static bool unbindElementArrayBuffer(GLuint elementArrayBufferId);
            static bool unbindArrayBuffer(GLuint arrayBufferId);
            static bool unbindVertexArray(GLuint vertexArrayId);

        protected:
            RendererOGL();

            virtual bool init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount, TextureFiltering newTextureFiltering) override;

            virtual void setSize(const Size2& newSize) override;

            bool ready = false;
            GLuint frameBuffer = 0;
            std::set<GLuint> clearedFrameBuffers;

            static GLuint currentTextureId[TEXTURE_LAYERS];
            static GLuint currentProgramId;
            static GLuint currentFrameBufferId;
            static GLuint currentElementArrayBufferId;
            static GLuint currentArrayBufferId;
            static GLuint currentVertexArrayId;

            Rectangle viewport;
        };
    } // namespace graphics
} // namespace ouzel
