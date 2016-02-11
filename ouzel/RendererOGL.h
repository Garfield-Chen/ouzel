// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#include "Renderer.h"

#if defined(OUZEL_PLATFORM_OSX)
    #include <OpenGL/gl3.h>
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#elif defined(OUZEL_PLATFORM_ANDROID)
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
    #define GL_GLEXT_PROTOTYPES 1
    #endif

    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
    #define glBindVertexArray glBindVertexArrayOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES
#elif defined(OUZEL_PLATFORM_ANDROID)
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;

    #define glGenVertexArrays glGenVertexArraysOESEXT
    #define glBindVertexArray glBindVertexArrayOESEXT
    #define glDeleteVertexArrays glDeleteVertexArraysOESEXT
#endif

namespace ouzel
{    
    class RendererOGL: public Renderer
    {
        friend Engine;
    public:
        virtual ~RendererOGL();
        
        bool initOpenGL(uint32_t width, uint32_t height, GLuint framebuffer);
        bool checkOpenGLErrors();
        
        virtual void setClearColor(Color color) override;
        
        virtual void resize(const Size2& size) override;
        
        virtual void clear() override;
        virtual void flush() override;
        
        virtual TexturePtr createTexture(const Size2& size, bool dynamic, bool mipmaps = true) override;
        virtual TexturePtr loadTextureFromFile(const std::string& filename, bool dynamic, bool mipmaps = true) override;
        virtual TexturePtr loadTextureFromData(const void* data, const Size2& size, bool dynamic, bool mipmaps = true) override;
        virtual VideoTexturePtr loadVideoTextureFromFile(const std::string& filename) override;
        virtual bool activateTexture(const TexturePtr& texture, uint32_t layer) override;
        virtual RenderTargetPtr createRenderTarget(const Size2& size, bool depthBuffer) override;
        virtual bool activateRenderTarget(const RenderTargetPtr& renderTarget) override;
        
        virtual ShaderPtr loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader, uint32_t vertexAttributes) override;
        virtual ShaderPtr loadShaderFromBuffers(const uint8_t* fragmentShader, uint32_t fragmentShaderSize, const uint8_t* vertexShader, uint32_t vertexShaderSize, uint32_t vertexAttributes) override;
        virtual bool activateShader(const ShaderPtr& shader) override;
        
        virtual MeshBufferPtr createMeshBuffer(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexSize, uint32_t vertexCount, bool dynamicVertexBuffer, uint32_t vertexAttributes) override;
        virtual bool drawMeshBuffer(const MeshBufferPtr& meshBuffer, uint32_t indexCount = 0, DrawMode drawMode = DrawMode::TRIANGLE_LIST) override;
        
        virtual bool saveScreenshot(const std::string& filename) override;
        
    private:
        RendererOGL();
        
        bool _ready = false;
        GLuint _framebuffer = 0;
    };
}
