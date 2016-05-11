// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Node.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Layer.h"
#include "Animator.h"
#include "Camera.h"
#include "Utils.h"
#include "MathUtils.h"
#include "Drawable.h"

namespace ouzel
{
    namespace scene
    {
        Node::Node()
        {

        }

        Node::~Node()
        {
            if (currentAnimator)
            {
                currentAnimator->stop();
                currentAnimator->release();
            }

            for (Drawable* drawable : drawables)
            {
                drawable->release();
            }
        }

        void Node::visit(const Matrix4& newTransformMatrix, bool parentTransformDirty)
        {
            if (parentTransformDirty)
            {
                updateTransform(newTransformMatrix);
            }

            if (visible)
            {
                if (transformDirty)
                {
                    calculateTransform();
                }

                // check if parent is layer
                bool isRoot = (parent == layer);

                if (children.empty())
                {
                    if (layer && (globalOrder || isRoot) && checkVisibility())
                    {
                        layer->addToDrawQueue(this);
                    }
                }
                else
                {
                    lock();

                    std::stable_sort(children.begin(), children.end(), [](Node* a, Node* b) {
                        return a->getZ() > b->getZ();
                    });

                    auto i = children.begin();

                    for (; i != children.end(); ++i)
                    {
                        Node* node = *i;

                        if (!node->remove)
                        {
                            if (node->getZ() < 0.0f)
                            {
                                node->visit(transform, updateChildrenTransform);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }

                    if (layer && (globalOrder || isRoot) && checkVisibility())
                    {
                        layer->addToDrawQueue(this);
                    }

                    for (; i != children.end(); ++i)
                    {
                        Node* node = *i;

                        if (!node->remove)
                        {
                            node->visit(transform, updateChildrenTransform);
                        }
                    }

                    unlock();
                }

                updateChildrenTransform = false;
            }
        }

        void Node::process()
        {
            if (children.empty())
            {
                draw();
            }
            else
            {
                lock();

                auto i = children.begin();
                Node* node;

                for (; i != children.end(); ++i)
                {
                    node = *i;

                    if (node->getZ() < 0.0f)
                    {
                        if (!node->isGlobalOrder() && node->isVisible() && node->checkVisibility())
                        {
                            node->draw();
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                draw();

                for (; i != children.end(); ++i)
                {
                    node = *i;

                    if (!node->isGlobalOrder() && node->isVisible() && node->checkVisibility())
                    {
                        node->draw();
                    }
                }

                unlock();
            }
        }

        void Node::draw()
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            if (layer)
            {
                if (layer->getCamera())
                {
                    graphics::Color drawColor(color.r, color.g, color.b, static_cast<uint8_t>(color.a * opacity));

                    for (Drawable* drawable : drawables)
                    {
                        if (drawable->isVisible())
                        {
                            drawable->draw(layer->getCamera()->getViewProjection(), transform, drawColor);
                        }
                    }
                }
            }
        }

        bool Node::addChild(Node* node)
        {
            if (NodeContainer::addChild(node))
            {
                node->addToLayer(layer);
                node->updateTransform(getTransform());

                return true;
            }
            else
            {
                return false;
            }
        }

        bool Node::removeFromParent()
        {
            if (parent)
            {
                parent->removeChild(this);
                return true;
            }

            return false;
        }

        void Node::setZ(float newZ)
        {
            z = newZ;

            // Currently z does not affect transformation
            //localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setGlobalOrder(bool newGlobalOrder)
        {
            globalOrder = newGlobalOrder;
        }

        void Node::setPosition(const Vector2& newPosition)
        {
            position = newPosition;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setRotation(float newRotation)
        {
            rotation = newRotation;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setScale(const Vector2& newScale)
        {
            scale = newScale;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setColor(const graphics::Color& newColor)
        {
            color = newColor;
        }

        void Node::setOpacity(float newOpacity)
        {
            opacity = clamp(newOpacity, 0.0f, 1.0f);
        }

        void Node::setFlipX(bool newFlipX)
        {
            flipX = newFlipX;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setFlipY(bool newFlipY)
        {
            flipY = newFlipY;

            localTransformDirty = transformDirty = inverseTransformDirty = true;
        }

        void Node::setVisible(bool newVisible)
        {
            visible = newVisible;
        }

        void Node::addToLayer(Layer* newLayer)
        {
            layer = newLayer;

            if (layer)
            {
                for (Node* child : children)
                {
                    child->addToLayer(layer);
                }
            }
        }

        void Node::removeFromLayer()
        {
            for (Node* child : children)
            {
                child->removeFromLayer();
            }
        }

        bool Node::pointOn(const Vector2& worldPosition) const
        {
            Vector2 localPosition = convertWorldToLocal(worldPosition);

            for (Drawable* drawable : drawables)
            {
                if (drawable->pointOn(localPosition))
                {
                    return true;
                }
            }

            return false;
        }

        bool Node::shapeOverlaps(const std::vector<Vector2>& edges) const
        {
            Matrix4 inverse = getInverseTransform();

            std::vector<Vector2> transformedEdges;

            for (const Vector2& edge : edges)
            {
                Vector3 transformedEdge = edge;

                inverse.transformPoint(transformedEdge);

                transformedEdges.push_back(Vector2(transformedEdge.x, transformedEdge.y));
            }

            for (Drawable* drawable : drawables)
            {
                if (drawable->shapeOverlaps(transformedEdges))
                {
                    return true;
                }
            }

            return false;
        }

        const Matrix4& Node::getTransform() const
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            return transform;
        }

        const Matrix4& Node::getInverseTransform() const
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            if (inverseTransformDirty)
            {
                calculateInverseTransform();
            }

            return inverseTransform;
        }

        void Node::updateTransform(const Matrix4& newParentTransform)
        {
            parentTransform = newParentTransform;
            transformDirty = inverseTransformDirty = true;
        }

        Vector2 Node::convertWorldToLocal(const Vector2& worldPosition) const
        {
            Vector3 localPosition = worldPosition;

            const Matrix4& currentInverseTransform = getInverseTransform();
            currentInverseTransform.transformPoint(localPosition);

            return Vector2(localPosition.x, localPosition.y);
        }

        Vector2 Node::convertLocalToWorld(const Vector2& localPosition) const
        {
            Vector3 worldPosition = localPosition;

            const Matrix4& currentTransform = getTransform();
            currentTransform.transformPoint(worldPosition);

            return Vector2(worldPosition.x, worldPosition.y);
        }

        bool Node::checkVisibility() const
        {
            // we must add this node to draw queue if it has children
            if (!children.empty())
            {
                return true;
            }

            if (layer)
            {
                for (Drawable* drawable : drawables)
                {
                    if (drawable->isVisible() &&
                        (drawable->getBoundingBox().isEmpty() ||
                         sharedEngine->getRenderer()->checkVisibility(getTransform(), drawable->getBoundingBox(), layer->getCamera())))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        void Node::animate(Animator* animator)
        {
            stopAnimation();
            currentAnimator = animator;

            if (currentAnimator)
            {
                currentAnimator->start(this);
                currentAnimator->retain();
            }
        }

        void Node::stopAnimation()
        {
            if (currentAnimator)
            {
                currentAnimator->stop();
                removeAnimation();
            }
        }

        void Node::removeAnimation()
        {
            if (currentAnimator)
            {
                currentAnimator->release();
                currentAnimator = nullptr;
            }
        }

        void Node::calculateLocalTransform() const
        {
            localTransform = Matrix4::IDENTITY;
            localTransform.translate(Vector3(position.x, position.y, 0.0f));
            localTransform.rotateZ(TAU - rotation);

            Vector3 realScale = Vector3(scale.x * (flipX ? -1.0f : 1.0f),
                                        scale.y * (flipY ? -1.0f : 1.0f),
                                        1.0f);

            localTransform.scale(realScale);

            localTransformDirty = false;
        }

        void Node::calculateTransform() const
        {
            if (localTransformDirty)
            {
                calculateLocalTransform();
            }

            transform = parentTransform * localTransform;
            transformDirty = false;

            updateChildrenTransform = true;
        }

        void Node::calculateInverseTransform() const
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            inverseTransform = transform;
            inverseTransform.invert();
            inverseTransformDirty = false;
        }

        void Node::addDrawable(Drawable* drawable)
        {
            drawables.push_back(drawable);
            drawable->setParentNode(this);
            drawable->retain();
        }

        void Node::removeDrawable(uint32_t index)
        {
            if (index >= drawables.size())
            {
                return;
            }

            Drawable* drawable = drawables[index];
            drawable->release();

            drawables.erase(drawables.begin() + index);
        }

        void Node::removeDrawable(Drawable* drawable)
        {
            for (auto i = drawables.begin(); i != drawables.end();)
            {
                if (*i == drawable)
                {
                    i = drawables.erase(i);
                    drawable->release();
                }
                else
                {
                    ++i;
                }
            }
        }

        void Node::removeAllDrawables()
        {
            for (Drawable* drawable : drawables)
            {
                drawable->release();
            }
            drawables.clear();
        }

    } // namespace scene
} // namespace ouzel
