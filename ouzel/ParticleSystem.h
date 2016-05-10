// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "Drawable.h"
#include "ParticleDefinition.h"
#include "Vector2.h"
#include "Color.h"
#include "Vertex.h"

namespace ouzel
{
    class UpdateCallback;

    namespace graphics
    {
        class Shader;
        class Texture;
        class MeshBuffer;
    }

    namespace scene
    {
        class SceneManager;

        struct Particle
        {
            float life = 0.0f;

            Vector2 position;

            float colorRed = 0.0f;
            float colorGreen = 0.0f;
            float colorBlue = 0.0f;
            float colorAlpha = 0.0f;

            float deltaColorRed = 0.0f;
            float deltaColorGreen = 0.0f;
            float deltaColorBlue = 0.0f;
            float deltaColorAlpha = 0.0f;

            float angle = 0.0f;
            float speed = 0.0f;

            float size = 0.0f;
            float deltaSize = 0.0f;

            float rotation = 0.0f;
            float deltaRotation = 0.0f;

            float radialAcceleration = 0.0f;
            float tangentialAcceleration = 0.0f;

            Vector2 direction;
            float radius = 0.0f;
            float degreesPerSecond = 0.0f;
            float deltaRadius = 0.0f;
        };

        class ParticleSystem: public Drawable
        {
        public:
            static ParticleSystem* createFromFile(const std::string& filename);

            ParticleSystem();
            virtual ~ParticleSystem();

            virtual void draw(const Matrix4& projectionMatrix, const Matrix4& transformMatrix, const graphics::Color& drawColor);
            virtual void update(float delta);

            virtual bool initFromFile(const std::string& filename);

            void resume();
            void stop();
            void reset();

            bool isRunning() const { return running; }
            bool isActive() const { return active; }

            void setPositionType(ParticleDefinition::PositionType newPositionType) { positionType = newPositionType; }
            ParticleDefinition::PositionType getPositionType() const { return positionType; }

        protected:
            void createParticleMesh();
            void updateParticleMesh();

            void emitParticles(uint32_t count);

            ParticleDefinition particleDefinition;
            ParticleDefinition::PositionType positionType;

            graphics::Shader* shader;
            graphics::Texture* texture;

            std::vector<Particle> particles;

            graphics::MeshBuffer* mesh;

            std::vector<uint16_t> indices;
            std::vector<graphics::VertexPCT> vertices;

            uint32_t particleCount = 0;

            float emitCounter = 0.0f;
            float elapsed = 0.0f;
            bool active = false;
            bool running = false;
            bool finished = false;

            bool needsMeshUpdate = false;

            UpdateCallback* updateCallback;
        };
    } // namespace scene
} // namespace ouzel
