// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RenderTargetOGL.h"
#include "TextureOGL.h"
#include "Engine.h"
#include "RendererOGL.h"

namespace ouzel
{
    namespace graphics
    {
        RenderTargetOGL::RenderTargetOGL()
        {

        }

        RenderTargetOGL::~RenderTargetOGL()
        {
            destroy();
        }

        void RenderTargetOGL::destroy()
        {
            if (depthBufferId)
            {
                glDeleteRenderbuffers(1, &depthBufferId);
                depthBufferId = 0;
            }

            if (frameBufferId)
            {
                glDeleteFramebuffers(1, &frameBufferId);
                frameBufferId = 0;
            }
        }

        bool RenderTargetOGL::init(const Size2& newSize, bool depthBuffer)
        {
            if (!RenderTarget::init(newSize, depthBuffer))
            {
                return false;
            }

            destroy();

            viewport = Rectangle(0.0f, 0.0f, newSize.width, newSize.height);

            GLuint oldFrameBufferId;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&oldFrameBufferId));

            glGenFramebuffers(1, &frameBufferId);

            RendererOGL::bindFrameBuffer(frameBufferId);

            TextureOGL* textureOGL = new TextureOGL();

            if (!textureOGL->init(size, false, false, true))
            {
                return false;
            }

            textureOGL->setFlipped(true);

            texture = textureOGL;

            RendererOGL::bindTexture(textureOGL->getTextureId(), 0);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         static_cast<GLsizei>(size.width),
                         static_cast<GLsizei>(size.height),
                         0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            if (depthBuffer)
            {
                glGenRenderbuffers(1, &depthBufferId);
                glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                      static_cast<GLsizei>(size.width),
                                      static_cast<GLsizei>(size.height));
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureOGL->getTextureId(), 0);

#ifdef OUZEL_SUPPORTS_OPENGL // TODO: fix this
            //GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
            //glDrawBuffers(1, drawBuffers);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
#endif

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
