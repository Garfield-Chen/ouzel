// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "Renderer.h"
#include "Engine.h"
#include "Texture.h"
#include "Node.h"
#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "EventHander.h"
#include "SceneManager.h"
#include "MeshBuffer.h"
#include "EventDispatcher.h"
#include "RenderTarget.h"
#include "BlendState.h"

namespace ouzel
{
    namespace graphics
    {
        Renderer::Renderer(Driver pDriver):
            driver(pDriver)
        {
            memset(activeTextures, 0, sizeof(activeTextures));
        }

        Renderer::~Renderer()
        {

        }

        bool Renderer::init(const Size2& newSize, bool newFullscreen, uint32_t newSampleCount, TextureFiltering newTextureFiltering)
        {
            size = newSize;
            fullscreen = newFullscreen;
            sampleCount = newSampleCount;
            textureFiltering = newTextureFiltering;

            return true;
        }

        void Renderer::clear()
        {
            drawCallCount = 0;
        }

        void Renderer::present()
        {
        }

        void Renderer::flush()
        {
        }

        void Renderer::setSize(const Size2& newSize)
        {
            size = newSize;

            sharedEngine->getSceneManager()->recalculateProjection();
        }

        void Renderer::setFullscreen(bool newFullscreen)
        {
            fullscreen = newFullscreen;
        }

        BlendState* Renderer::createBlendState(bool enableBlending,
                                                 BlendState::BlendFactor colorBlendSource, BlendState::BlendFactor colorBlendDest,
                                                 BlendState::BlendOperation colorOperation,
                                                 BlendState::BlendFactor alphaBlendSource, BlendState::BlendFactor alphaBlendDest,
                                                 BlendState::BlendOperation alphaOperation)
        {
            BlendState* blendState = new BlendState();
            if (!blendState->init(enableBlending,
                                  colorBlendSource, colorBlendDest,
                                  colorOperation,
                                  alphaBlendSource, alphaBlendDest,
                                  alphaOperation))
            {
                blendState->release();
                blendState = nullptr;
            }

            return blendState;
        }

        bool Renderer::activateBlendState(BlendState* blendState)
        {
            activeBlendState = blendState;

            return true;
        }

        bool Renderer::activateTexture(Texture* texture, uint32_t layer)
        {
            activeTextures[layer] = texture;

            return true;
        }

        bool Renderer::activateRenderTarget(RenderTarget* renderTarget)
        {
            activeRenderTarget = renderTarget;

            return true;
        }

        bool Renderer::activateShader(Shader* shader)
        {
            activeShader = shader;

            return true;
        }

        MeshBuffer* Renderer::createMeshBuffer()
        {
            MeshBuffer* meshBuffer = new MeshBuffer();

            if (!meshBuffer->init())
            {
                meshBuffer->release();
                meshBuffer = nullptr;
            }

            return meshBuffer;
        }

        bool Renderer::drawMeshBuffer(MeshBuffer* meshBuffer, uint32_t indexCount, DrawMode drawMode, uint32_t startIndex)
        {
            OUZEL_UNUSED(drawMode);
            OUZEL_UNUSED(startIndex);

            if (activeShader)
            {
                if (meshBuffer->getVertexAttributes() != activeShader->getVertexAttributes())
                {
                    return false;
                }

                if (indexCount > meshBuffer->getIndexCount())
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            ++drawCallCount;

            return true;
        }

        Vector2 Renderer::viewToScreenLocation(const Vector2& position)
        {
            float x = 2.0f * position.x / size.width - 1.0f;
            float y = 2.0f * (size.height - position.y) / size.height - 1.0f;

            return Vector2(x, y);
        }

        Vector2 Renderer::screenToViewLocation(const Vector2& position)
        {
            float x = (position.x + 1.0f) / 2.0f * size.width;
            float y = size.height - (position.y + 1.0f) / 2.0f * size.height;

            return Vector2(x, y);
        }

        bool Renderer::checkVisibility(const Matrix4& transform, const AABB2& boundingBox, scene::Camera* camera)
        {
            Rectangle visibleRect(0.0f, 0.0f, size.width, size.height);

            // transform center point to screen space
            Vector2 diff = boundingBox.max - boundingBox.min;

            Vector3 v3p(boundingBox.min.x + diff.x / 2.0f, boundingBox.min.y + diff.y / 2.0f, 0.0f);
            diff *= camera->getZoom();
            diff.x *= camera->getContentScale().x;
            diff.y *= camera->getContentScale().y;

            transform.transformPoint(v3p);

            Vector2 v2p = camera->projectPoint(v3p);

            Size2 halfSize(diff.x / 2.0f, diff.y / 2.0f);

            // convert content size to world coordinates
            Size2 halfWorldSize;

            halfWorldSize.width = std::max(fabsf(halfSize.width * transform.m[0] + halfSize.height * transform.m[4]), fabsf(halfSize.width * transform.m[0] - halfSize.height * transform.m[4]));
            halfWorldSize.height = std::max(fabsf(halfSize.width * transform.m[1] + halfSize.height * transform.m[5]), fabsf(halfSize.width * transform.m[1] - halfSize.height * transform.m[5]));

            // enlarge visible rect half size in screen coord
            visibleRect.x -= halfWorldSize.width;
            visibleRect.y -= halfWorldSize.height;
            visibleRect.width += halfWorldSize.width * 2.0f;
            visibleRect.height += halfWorldSize.height * 2.0f;

            return visibleRect.containsPoint(v2p);
        }
    } // namespace graphics
} // namespace ouzel
