// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <limits>

#include "core/CompileConfig.h"
#if OUZEL_PLATFORM_ANDROID
#include <cpu-features.h>
#endif

#define OUZEL_UNUSED(x) (void)(x)

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    class AnrdoidNEONChecker
    {
    public:
        AnrdoidNEONChecker()
        {
            if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM && (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
                neonAvailable = true;
            else
                neonAvailable = false;
        }

        bool isNEONAvailable() const { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AnrdoidNEONChecker anrdoidNEONChecker;
#endif

    template<typename T> size_t vectorDataSize(const typename std::vector<T>& vec)
    {
        return sizeof(T) * vec.size();
    }

    enum LogLevel
    {
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_VERBOSE
    };

    void setLogLevel(LogLevel level);
    void log(LogLevel level, const char* format, ...);

    uint32_t random(uint32_t min = 0, uint32_t max = std::numeric_limits<uint32_t>::max());
    float randomf(float min = 0.0f, float max = 1.0f);

    inline uint32_t readUInt32Big(const uint8_t* buffer)
    {
        uint32_t result = static_cast<uint32_t>(buffer[3]) |
                          static_cast<uint32_t>(buffer[2])<<8 |
                          static_cast<uint32_t>(buffer[1])<<16 |
                          static_cast<uint32_t>(buffer[0])<<24;

        return result;
    };

    inline uint32_t readUInt32Little(const uint8_t* buffer)
    {
        uint32_t result = static_cast<uint32_t>(buffer[0]) |
                          static_cast<uint32_t>(buffer[1])<<8 |
                          static_cast<uint32_t>(buffer[2])<<16 |
                          static_cast<uint32_t>(buffer[3])<<24;

        return result;
    };

    inline uint16_t readUInt16Big(const uint8_t* buffer)
    {
        uint32_t result = static_cast<uint32_t>(buffer[0]) |
                          static_cast<uint32_t>(buffer[1])<<8;

        return static_cast<uint16_t>(result);
    };

    inline uint16_t readUInt16Little(const uint8_t* buffer)
    {
        uint32_t result = static_cast<uint32_t>(buffer[0]) |
                          static_cast<uint32_t>(buffer[1])<<8;

        return static_cast<uint16_t>(result);
    };
}
