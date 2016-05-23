// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include "Language.h"
#include "Engine.h"
#include "Utils.h"

namespace ouzel
{
    struct TranslationInfo
    {
        uint32_t stringLength;
        uint32_t stringOffset;

        uint32_t translationLength;
        uint32_t translationOffset;
    };

    Language* Language::createFromFile(const std::string& filename)
    {
        Language* result = new Language();

        if (!result->initFromFile(filename))
        {
            result->release();
            result = nullptr;
        }

        return result;
    }

    bool Language::initFromFile(const std::string& filename)
    {
        std::vector<uint8_t> data;

        if (!sharedEngine->getFileSystem()->loadFile(filename, data))
        {
            return false;
        }

        uint32_t offset = 0;

        if (data.size() < 5 * sizeof(uint32_t))
        {
            return false;
        }

        uint32_t magic = *reinterpret_cast<uint32_t*>(data.data() + offset);
        offset += sizeof(magic);

        if (magic != 0x950412DE)
        {
            log("Wrong magic %x", magic);
            return false;
        }

        uint32_t version = *reinterpret_cast<uint32_t*>(data.data() + offset);
        offset += sizeof(version);
        OUZEL_UNUSED(version);

        uint32_t stringCount = *reinterpret_cast<uint32_t*>(data.data() + offset);
        offset += sizeof(stringCount);

        std::vector<TranslationInfo> translations(stringCount);

        uint32_t stringsOffset = *reinterpret_cast<uint32_t*>(data.data() + offset);
        offset += sizeof(stringsOffset);

        uint32_t translationsOffset = *reinterpret_cast<uint32_t*>(data.data() + offset);
        offset += sizeof(translationsOffset);

        offset = stringsOffset;

        if (data.size() < offset + 2 * sizeof(uint32_t) * stringCount)
        {
            return false;
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            translations[i].stringLength = *reinterpret_cast<uint32_t*>(data.data() + offset);
            offset += sizeof(translations[i].stringLength);

            translations[i].stringOffset = *reinterpret_cast<uint32_t*>(data.data() + offset);
            offset += sizeof(translations[i].stringOffset);
        }

        offset = translationsOffset;

        if (data.size() < offset + 2 * sizeof(uint32_t) * stringCount)
        {
            return false;
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            translations[i].translationLength = *reinterpret_cast<uint32_t*>(data.data() + offset);
            offset += sizeof(translations[i].translationLength);

            translations[i].translationOffset = *reinterpret_cast<uint32_t*>(data.data() + offset);
            offset += sizeof(translations[i].translationOffset);
        }

        for (uint32_t i = 0; i < stringCount; ++i)
        {
            if (data.size() < translations[i].stringOffset + translations[i].stringLength ||
                data.size() < translations[i].translationOffset + translations[i].translationLength)
            {
                return false;
            }

            std::string str(reinterpret_cast<char*>(data.data() + translations[i].stringOffset), translations[i].stringLength);
            std::string translation(reinterpret_cast<char*>(data.data() + translations[i].translationOffset), translations[i].translationLength);

            strings[str] = translation;
        }

        return true;
    }

    std::string Language::getString(const std::string& str)
    {
        auto i = strings.find(str);

        if (i != strings.end())
        {
            return i->second;
        }
        else
        {
            return str;
        }
    }
}
