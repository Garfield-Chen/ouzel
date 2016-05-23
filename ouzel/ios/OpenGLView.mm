// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#import "OpenGLView.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "Engine.h"
#include "RendererOGL.h"
#include "Input.h"
#include "Utils.h"

using namespace ouzel;

@implementation OpenGLView

-(id)initWithFrame:(CGRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        std::shared_ptr<graphics::RendererOGL> rendererOGL = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());

        eaglLayer = (CAEAGLLayer*)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

        if (context)
        {
            rendererOGL->setAPIVersion(3);
            log("Using OpenGL ES 3");
        }
        else
        {
            log("Failed to create OpenGL ES 3 rendering context");

            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

            if (context)
            {
                rendererOGL->setAPIVersion(2);
                log("Using OpenGL ES 2");
            }
            else
            {
                ouzel::log("Failed to initialize OpenGL ES 2 rendering context");
                return Nil;
            }
        }

        if (![EAGLContext setCurrentContext:context])
        {
            ouzel::log("Failed to set current OpenGL rendering context");
            return Nil;
        }

        // render buffer
        glGenRenderbuffers(1, &colorRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];

        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backingHeight);

        // frame buffer
        glGenFramebuffers(1, &frameBuffer);
        graphics::RendererOGL::bindFrameBuffer(frameBuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER, colorRenderBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            log("Failed to create framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
            return Nil;
        }

        // display link
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(idle:)];
        [displayLink setFrameInterval:1.0f];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        graphics::RendererOGL* renderer = static_cast<graphics::RendererOGL*>(sharedEngine->getRenderer());
        renderer->setFrameBuffer(frameBuffer);
    }

    return self;
}

-(void)dealloc
{
    [displayLink invalidate];
    [displayLink release];

    if ([EAGLContext currentContext] == context)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [context release];

    if (frameBuffer) glDeleteFramebuffers(1, &frameBuffer);
    frameBuffer = 0;
    if (colorRenderBuffer) glDeleteRenderbuffers(1, &colorRenderBuffer);
    colorRenderBuffer = 0;

    [super dealloc];
}

+(Class)layerClass
{
    return [CAEAGLLayer class];
}

-(void)idle:(id)sender
{
    OUZEL_UNUSED(sender);
    if (![EAGLContext setCurrentContext:context])
    {
        ouzel::log("Failed to set current OpenGL context");
    }

    sharedEngine->run();

    [context presentRenderbuffer:GL_RENDERBUFFER];
}

-(void)touchesBegan:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchBegin(reinterpret_cast<uint64_t>(touch),
                                             sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

-(void)touchesMoved:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchMove(reinterpret_cast<uint64_t>(touch),
                                            sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

-(void)touchesEnded:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchEnd(reinterpret_cast<uint64_t>(touch),
                                           sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

-(void)touchesCancelled:(NSSet*)touches withEvent:(::UIEvent*)event
{
    OUZEL_UNUSED(event);
    for (UITouch* touch in touches)
    {
        CGPoint location = [touch locationInView:self];

        sharedEngine->getInput()->touchCancel(reinterpret_cast<uint64_t>(touch),
                                              sharedEngine->getRenderer()->viewToScreenLocation(Vector2(location.x, location.y)));
    }
}

@end
