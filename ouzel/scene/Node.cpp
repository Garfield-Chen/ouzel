// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Node.h"
#include "core/Engine.h"
#include "SceneManager.h"
#include "Layer.h"
#include "animators/Animator.h"
#include "Camera.h"
#include "utils/Utils.h"
#include "math/MathUtils.h"
#include "Component.h"

namespace ouzel
{
    namespace scene
    {
        Node::Node()
        {
        }

        Node::~Node()
        {
            for (const ComponentPtr& component : components)
            {
                component->setNode(nullptr);
            }
        }

        void Node::visit(const Matrix4& newTransformMatrix, bool parentTransformDirty, const LayerPtr& currentLayer, float depth)
        {
            if (parentTransformDirty)
            {
                updateTransform(newTransformMatrix);
            }

            if (transformDirty)
            {
                calculateTransform();
            }

            if (currentLayer)
            {
                if (currentLayer->checkVisibility(std::static_pointer_cast<Node>(shared_from_this())))
                {
                    currentLayer->addToDrawQueue(std::static_pointer_cast<Node>(shared_from_this()), depth + z);
                }

                for (const NodePtr& child : children)
                {
                    if (!child->isHidden())
                    {
                        child->visit(transform, updateChildrenTransform, currentLayer, depth + z);
                    }
                }
            }

            updateChildrenTransform = false;
        }

        void Node::draw(const LayerPtr& currentLayer)
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            if (currentLayer)
            {
                if (currentLayer->getCamera())
                {
                    graphics::Color drawColor(color.r, color.g, color.b, static_cast<uint8_t>(color.a * opacity));

                    for (const ComponentPtr& component : components)
                    {
                        if (!component->isHidden())
                        {
                            component->draw(currentLayer->getCamera()->getViewProjection(),
                                            transform,
                                            drawColor,
                                            currentLayer->getRenderTarget());
                        }
                    }
                }
            }
        }

        void Node::drawWireframe(const LayerPtr& currentLayer)
        {
            if (transformDirty)
            {
                calculateTransform();
            }

            if (currentLayer)
            {
                if (currentLayer->getCamera())
                {
                    graphics::Color drawColor(color.r, color.g, color.b, static_cast<uint8_t>(color.a * opacity));

                    for (const ComponentPtr& component : components)
                    {
                        if (!component->isHidden())
                        {
                            component->drawWireframe(currentLayer->getCamera()->getViewProjection(),
                                                     transform,
                                                     drawColor,
                                                     currentLayer->getRenderTarget());
                        }
                    }
                }
            }
        }

        bool Node::addChild(const NodePtr& node)
        {
            if (NodeContainer::addChild(node))
            {
                node->updateTransform(getTransform());

                return true;
            }
            else
            {
                return false;
            }
        }

        void Node::setZ(float newZ)
        {
            z = newZ;

            // Currently z does not affect transformation
            //localTransformDirty = transformDirty = inverseTransformDirty = true;
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

        void Node::setHidden(bool newHidden)
        {
            hidden = newHidden;
        }

        bool Node::pointOn(const Vector2& worldPosition) const
        {
            Vector2 localPosition = convertWorldToLocal(worldPosition);

            for (const ComponentPtr& component : components)
            {
                if (component->pointOn(localPosition))
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

            for (const ComponentPtr& component : components)
            {
                if (component->shapeOverlaps(transformedEdges))
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

        void Node::animate(const AnimatorPtr& animator)
        {
            stopAnimation();
            currentAnimator = animator;

            if (currentAnimator)
            {
                currentAnimator->start(std::static_pointer_cast<Node>(shared_from_this()));
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
            currentAnimator.reset();
        }

        void Node::calculateLocalTransform() const
        {
            localTransform = Matrix4::IDENTITY;
            localTransform.translate(Vector3(position.x, position.y, 0.0f));
            if (rotation != 0.0f) localTransform.rotateZ(-rotation);

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

        bool Node::addComponent(const ComponentPtr& component)
        {
            if (component->isAddedToNode())
            {
                return false;
            }

            components.push_back(component);
            component->setNode(this);

            return true;
        }

        bool Node::removeComponent(uint32_t index)
        {
            if (index >= components.size())
            {
                return false;
            }

            const ComponentPtr& component = components[index];
            component->setNode(nullptr);

            components.erase(components.begin() + static_cast<int>(index));

            return true;
        }

        bool Node::removeComponent(const ComponentPtr& component)
        {
            for (auto i = components.begin(); i != components.end();)
            {
                if (*i == component)
                {
                    component->setNode(nullptr);
                    components.erase(i);
                    return true;
                }
                else
                {
                    ++i;
                }
            }

            return false;
        }

        void Node::removeAllComponents()
        {
            components.clear();
        }

    } // namespace scene
} // namespace ouzel
