// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <atomic>
#include <mutex>
#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS
    #include <OpenGL/gl3.h>
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif OUZEL_PLATFORM_ANDROID
    #include <GLES2/gl2platform.h>
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#elif OUZEL_PLATFORM_LINUX
    #define GL_GLEXT_PROTOTYPES 1
    #include <GL/gl.h>
    #include <GL/glx.h>
    #include <GL/glext.h>
#elif OUZEL_PLATFORM_RASPBIAN
    #define GL_GLEXT_PROTOTYPES 1
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <EGL/egl.h>
#endif

#include "graphics/RenderTarget.h"
#include "math/Size2.h"
#include "math/Rectangle.h"

namespace ouzel
{
    namespace graphics
    {
        class RendererOGL;

        class RenderTargetOGL: public RenderTarget
        {
            friend RendererOGL;
        public:
            virtual ~RenderTargetOGL();
            void free() override;

            virtual bool init(const Size2& newSize, bool depthBuffer) override;

            virtual void setClearColor(Color color) override;

            GLuint getFrameBufferId() const { return frameBufferId; }
            const Rectangle& getViewport() const { return viewport; }

            GLbitfield getClearMask() const { return clearMask; }
            const GLfloat* getFrameBufferClearColor() const { return frameBufferClearColor; }

        protected:
            RenderTargetOGL();
            virtual bool upload() override;

            GLuint frameBufferId = 0;
            GLuint depthBufferId = 0;
            Rectangle viewport;

            GLbitfield clearMask = 0;
            GLfloat frameBufferClearColor[4];

            std::atomic<bool> dirty;
            std::mutex dataMutex;
        };
    } // namespace graphics
} // namespace ouzel
