// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include "Camera.h"
#include "Engine.h"
#include "Renderer.h"
#include "Layer.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Matrix4.h"
#include "Utils.h"

namespace ouzel
{
    namespace scene
    {
        Camera::Camera()
        {

        }

        Camera::~Camera()
        {

        }

        void Camera::setZoom(float newZoom)
        {
            zoom = newZoom;

            if (zoom < 0.1f)
            {
                zoom = 0.1f;
            }

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Camera::recalculateProjection()
        {
            Size2 screenSize;

            if (layer)
            {
                if (graphics::RenderTarget* renderTarget = layer->getRenderTarget())
                {
                    screenSize = renderTarget->getTexture()->getSize();
                }
                else
                {
                    screenSize = sharedEngine->getRenderer()->getSize();
                }
            }
            else
            {
                return;
            }

            if (screenSize.width == 0.0f || screenSize.height == 0.0f)
            {
                return;
            }

            if (targetContentSize.width != 0.0f && targetContentSize.height != 0.0f)
            {
                contentSize = targetContentSize;
                contentScale.x = screenSize.width / contentSize.width;
                contentScale.y = screenSize.height / contentSize.height;

                switch (scaleMode)
                {
                    case ScaleMode::None:
                    {
                        break;
                    }
                    case ScaleMode::ExactFit:
                    {
                        contentScale.x = 1.0f;
                        contentScale.y = 1.0f;
                        break;
                    }
                    case ScaleMode::NoBorder:
                    {
                        contentScale.x = contentScale.y = fmaxf(contentScale.x, contentScale.y);
                        break;
                    }
                    case ScaleMode::ShowAll:
                    {
                        contentScale.x = contentScale.y = fminf(contentScale.x, contentScale.y);
                        break;
                    }
                }
            }
            else
            {
                contentSize = screenSize;
                contentScale = Vector2(1.0f, 1.0f);
            }

            Matrix4::createOrthographic(screenSize.width / contentScale.x, screenSize.height / contentScale.y, -1.0f, 1.0f, projection);

            inverseProjection = projection;
            inverseProjection.invert();

            viewProjectionDirty = true;
        }

        const Matrix4& Camera::getViewProjection() const
        {
            if (viewProjectionDirty || transformDirty)
            {
                viewProjection = projection * getTransform();
            }

            return viewProjection;
        }

        void Camera::calculateLocalTransform() const
        {
            Matrix4 translationMatrix;
            translationMatrix.translate(-position.x, -position.y, 0.0f);

            Matrix4 rotationMatrix;
            rotationMatrix.rotate(Vector3(0.0f, 0.0f, -1.0f), -rotation);

            Matrix4 scaleMatrix;
            scaleMatrix.scale(zoom);

            localTransform = scaleMatrix * rotationMatrix * translationMatrix;

            localTransformDirty = false;
        }

        Vector2 Camera::convertScreenToWorld(const Vector2& position)
        {
            Matrix4 projViewMatrix = projection * getTransform();
            Matrix4 inverseViewMatrix = projViewMatrix;
            inverseViewMatrix.invert();

            Vector3 result = Vector3(position.x, position.y, 0.0f);
            inverseViewMatrix.transformPoint(result);

            return Vector2(result.x, result.y);
        }

        Vector2 Camera::convertWorldToScreen(const Vector2& position)
        {
            Matrix4 projViewMatrix = projection * getTransform();

            Vector3 result = Vector3(position.x, position.y, 0.0f);
            projViewMatrix.transformPoint(result);

            return Vector2(result.x, result.y);
        }

        Vector2 Camera::projectPoint(const Vector3& src) const
        {
            Vector2 screenPos;

            auto viewport = sharedEngine->getRenderer()->getSize();
            Vector4 clipPos;

            getViewProjection().transformVector(Vector4(src.x, src.y, src.z, 1.0f), clipPos);

            assert(clipPos.w != 0.0f);
            Vector2 ndc(clipPos.x / clipPos.w, clipPos.y / clipPos.w);

            screenPos.x = (ndc.x + 1.0f) * 0.5f * viewport.width;
            screenPos.y = (ndc.y + 1.0f) * 0.5f * viewport.height;

            return screenPos;
        }

        void Camera::setScaleMode(ScaleMode newScaleMode)
        {
            scaleMode = newScaleMode;
            recalculateProjection();
        }

        void Camera::setTargetContentSize(const Size2& newTargetContentSize)
        {
            targetContentSize = newTargetContentSize;
            recalculateProjection();
        }
    } // namespace scene
} // namespace ouzel
