// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "ReferenceCounted.h"
#include "NodeContainer.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Rectangle.h"
#include "AABB2.h"

namespace ouzel
{
    namespace scene
    {
        class SceneManager;
        class Animator;

        class Node: public ReferenceCounted, public NodeContainer
        {
            friend SceneManager;
            friend NodeContainer;
            friend Layer;
        public:
            Node();
            virtual ~Node();

            virtual void visit(const Matrix4& parentTransform, bool parentTransformDirty);
            virtual void process();
            virtual void draw();

            virtual bool addChild(Node* node) override;
            virtual bool hasParent() const { return _parent != nullptr; }
            virtual NodeContainer* getParent() const { return _parent; }
            virtual bool removeFromParent();

            virtual void setZ(float z);
            virtual float getZ() const { return _z; }

            virtual void setGlobalOrder(bool globalOrder);
            virtual bool isGlobalOrder() const { return _globalOrder; }

            virtual void setPosition(const Vector2& position);
            virtual const Vector2& getPosition() const { return _position; }

            virtual void setRotation(float rotation);
            virtual float getRotation() const { return _rotation; }

            virtual void setScale(const Vector2& scale);
            virtual const Vector2& getScale() const { return _scale; }

            virtual void setOpacity(float opacity);
            virtual float getOpacity() const { return _opacity; }

            virtual void setFlipX(bool flipX);
            virtual bool getFlipX() const { return _flipX; }

            virtual void setFlipY(bool flipY);
            virtual bool getFlipY() const { return _flipY; }

            virtual void setPickable(bool pickable) { _pickable = pickable; }
            virtual bool isPickable() const { return _pickable; }

            virtual void setVisible(bool visible);
            virtual bool isVisible() const { return _visible; }

            virtual const AABB2& getBoundingBox() const { return _boundingBox; }

            virtual bool pointOn(const Vector2& position) const;
            virtual bool rectangleOverlaps(const Rectangle& rectangle) const;

            virtual const Matrix4& getTransform() const;
            const Matrix4& getInverseTransform() const;

            virtual void updateTransform(const Matrix4& parentTransform);

            Vector2 convertWorldToLocal(const Vector2& position) const;
            Vector2 convertLocalToWorld(const Vector2& position) const;

            virtual bool checkVisibility() const;

            virtual void animate(Animator* animator);
            virtual Animator* getAnimator() const { return _currentAnimator; }
            virtual void stopAnimation();
            virtual void removeAnimation();

            void receiveInput(bool receiveInput) { _receiveInput = receiveInput; }
            bool isReceivingInput() const { return _receiveInput; }

        protected:
            virtual void addToLayer(Layer* layer);
            virtual void removeFromLayer();

            virtual void calculateLocalTransform() const;
            virtual void calculateTransform() const;

            virtual void calculateInverseTransform() const;

            Matrix4 _parentTransform = Matrix4::IDENTITY;
            mutable Matrix4 _transform;
            mutable bool _transformDirty = true;
            mutable Matrix4 _inverseTransform;
            mutable bool _inverseTransformDirty = true;

            mutable Matrix4 _localTransform;
            mutable bool _localTransformDirty = true;

            mutable bool _updateChildrenTransform = true;

            //TODO: transform to parent and transform to parent

            Vector2 _position;
            float _rotation = 0.0f;
            Vector2 _scale = Vector2(1.0f, 1.0f);
            float _opacity = 1.0f;
            float _z = 0.0f;
            bool _globalOrder = true;

            AABB2 _boundingBox;

            bool _flipX = false;
            bool _flipY = false;

            bool _pickable = true;
            bool _visible = true;
            bool _receiveInput = false;

            NodeContainer* _parent = nullptr;
            Layer* _layer;

            Animator* _currentAnimator;
            bool _remove = false;
        };
    } // namespace scene
} // namespace ouzel
