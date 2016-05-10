// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <rapidjson/rapidjson.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "SpriteFrame.h"
#include "Engine.h"
#include "Cache.h"
#include "Texture.h"
#include "Utils.h"
#include "File.h"

namespace ouzel
{
    namespace scene
    {
        std::vector<SpriteFrame*> SpriteFrame::loadSpriteFrames(const std::string& filename, bool mipmaps)
        {
            std::vector<SpriteFrame*> frames;

            File file(filename, File::Mode::READ, false);

            if (!file)
            {
                log("Failed to open %s", filename.c_str());
                return frames;
            }

            rapidjson::FileReadStream is(file.getFile(), TEMP_BUFFER, sizeof(TEMP_BUFFER));

            rapidjson::Document document;
            document.ParseStream<0>(is);

            if (document.HasParseError())
            {
                log("Failed to parse %s", filename.c_str());
                return frames;
            }

            const rapidjson::Value& metaObject = document["meta"];
            const rapidjson::Value& sizeObject = metaObject["size"];

            Size2 textureSize(static_cast<float>(sizeObject["w"].GetInt()),
                              static_cast<float>(sizeObject["h"].GetInt()));

            graphics::Texture* texture = sharedEngine->getCache()->getTexture(metaObject["image"].GetString(), false, mipmaps);

            const rapidjson::Value& framesArray = document["frames"];

            frames.reserve(framesArray.Size());

            for (uint32_t index = 0; index < static_cast<uint32_t>(framesArray.Size()); ++index)
            {
                const rapidjson::Value& frameObject = framesArray[index];

                const rapidjson::Value& rectangleObject = frameObject["frame"];

                Rectangle rectangle(static_cast<float>(rectangleObject["x"].GetInt()),
                                    static_cast<float>(rectangleObject["y"].GetInt()),
                                    static_cast<float>(rectangleObject["w"].GetInt()),
                                    static_cast<float>(rectangleObject["h"].GetInt()));

                bool rotated = frameObject["rotated"].GetBool();

                const rapidjson::Value& sourceSizeObject = frameObject["sourceSize"];

                Size2 sourceSize(static_cast<float>(sourceSizeObject["w"].GetInt()),
                                 static_cast<float>(sourceSizeObject["h"].GetInt()));

                const rapidjson::Value& spriteSourceSizeObject = frameObject["spriteSourceSize"];

                Vector2 sourceOffset(static_cast<float>(spriteSourceSizeObject["x"].GetInt()),
                                     static_cast<float>(spriteSourceSizeObject["y"].GetInt()));

                const rapidjson::Value& pivotObject = frameObject["pivot"];

                Vector2 pivot(static_cast<float>(pivotObject["x"].GetDouble()),
                              static_cast<float>(pivotObject["y"].GetDouble()));
                
                frames.push_back(create(rectangle, texture, rotated, sourceSize, sourceOffset, pivot));
            }

            return frames;
        }

        SpriteFrame* SpriteFrame::create(const Rectangle& rectangle, graphics::Texture* texture, bool rotated, const Size2& sourceSize, const Vector2& sourceOffset, const Vector2& pivot)
        {
            std::vector<uint16_t> indices = {0, 1, 2, 1, 3, 2};

            Vector2 textCoords[4];
            Vector2 finalOffset(-sourceSize.width * pivot.x + sourceOffset.x,
                                -sourceSize.height * pivot.y + (sourceSize.height - rectangle.height - sourceOffset.y));

            const Size2& textureSize = texture->getSize();

            if (!rotated)
            {
                Vector2 leftTop(rectangle.x / textureSize.width,
                                rectangle.y / textureSize.height);

                Vector2 rightBottom((rectangle.x + rectangle.width) / textureSize.width,
                                    (rectangle.y + rectangle.height) / textureSize.height);

                if (texture->isFlipped())
                {
                    leftTop.y = 1.0f - leftTop.y;
                    rightBottom.y = 1.0f - rightBottom.y;
                }

                textCoords[0] = Vector2(leftTop.x, rightBottom.y);
                textCoords[1] = Vector2(rightBottom.x, rightBottom.y);
                textCoords[2] = Vector2(leftTop.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, leftTop.y);
            }
            else
            {
                Vector2 leftTop = Vector2(rectangle.x / textureSize.width,
                                          rectangle.y / textureSize.height);

                Vector2 rightBottom = Vector2((rectangle.x + rectangle.height) / textureSize.width,
                                              (rectangle.y + rectangle.width) / textureSize.height);

                if (texture->isFlipped())
                {
                    leftTop.y = 1.0f - leftTop.y;
                    rightBottom.y = 1.0f - rightBottom.y;
                }

                textCoords[0] = Vector2(leftTop.x, leftTop.y);
                textCoords[1] = Vector2(leftTop.x, rightBottom.y);
                textCoords[2] = Vector2(rightBottom.x, leftTop.y);
                textCoords[3] = Vector2(rightBottom.x, rightBottom.y);
            }

            Rectangle newRectangle = Rectangle(finalOffset.x, finalOffset.y,
                                               rectangle.width, rectangle.height);

            std::vector<graphics::VertexPCT> vertices = {
                graphics::VertexPCT(Vector3(finalOffset.x, finalOffset.y, 0.0f), graphics::Color(255, 255, 255, 255), textCoords[0]),
                graphics::VertexPCT(Vector3(finalOffset.x + rectangle.width, finalOffset.y, 0.0f), graphics::Color(255, 255, 255, 255), textCoords[1]),
                graphics::VertexPCT(Vector3(finalOffset.x, finalOffset.y + rectangle.height, 0.0f),  graphics::Color(255, 255, 255, 255), textCoords[2]),
                graphics::VertexPCT(Vector3(finalOffset.x + rectangle.width, finalOffset.y + rectangle.height, 0.0f),  graphics::Color(255, 255, 255, 255), textCoords[3])
            };

            graphics::MeshBuffer* meshBuffer = (sharedEngine->getRenderer()->createMeshBufferFromData(indices.data(), sizeof(uint16_t),
                                                                                                        static_cast<uint32_t>(indices.size()), false,
                                                                                                        vertices.data(), graphics::VertexPCT::ATTRIBUTES,
                                                                                                        static_cast<uint32_t>(vertices.size()), true));

            SpriteFrame* frame = new SpriteFrame(newRectangle, meshBuffer, texture);

            return frame;
        }
    } // scene
} // ouzel
