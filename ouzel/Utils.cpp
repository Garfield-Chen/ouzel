// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdarg>
#include <random>
#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS) || defined(OUZEL_PLATFORM_ANDROID) || defined(OUZEL_PLATFORM_LINUX)
#include <sys/time.h>
#endif

#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include <sys/syslog.h>
#endif

#ifdef OUZEL_PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <strsafe.h>
#endif

#ifdef OUZEL_PLATFORM_ANDROID
#include <android/log.h>
#endif

#include "Utils.h"

namespace ouzel
{
    char TEMP_BUFFER[65536];

    void log(const char* format, ...)
    {
        va_list list;
        va_start(list, format);

        vsprintf(TEMP_BUFFER, format, list);

        va_end(list);

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_LINUX)
        printf("%s\n", TEMP_BUFFER);
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        syslog(LOG_WARNING, "%s", TEMP_BUFFER);
        printf("%s\n", TEMP_BUFFER);
#elif defined(OUZEL_PLATFORM_WINDOWS)
        wchar_t szBuffer[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, TEMP_BUFFER, -1, szBuffer, MAX_PATH);
        StringCchCat(szBuffer, sizeof(szBuffer), L"\n");
        OutputDebugString(szBuffer);
#elif defined(OUZEL_PLATFORM_ANDROID)
        __android_log_print(ANDROID_LOG_DEBUG, "Ouzel", "%s", TEMP_BUFFER);
#endif
    }

    uint64_t getCurrentMicroSeconds()
    {
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS) || defined(OUZEL_PLATFORM_ANDROID) || defined(OUZEL_PLATFORM_LINUX)
        struct timeval currentTime;

        gettimeofday(&currentTime, NULL);
        return static_cast<uint64_t>(currentTime.tv_sec) * 1000000UL + static_cast<uint64_t>(currentTime.tv_usec);
#elif defined(OUZEL_PLATFORM_WINDOWS)

        static double invFrequency = 0.0;
        LARGE_INTEGER li;

        if (invFrequency == 0.0f)
        {
            if (!QueryPerformanceFrequency(&li))
            {
                log("Failed to query frequency");
                return 0;
            }
            invFrequency = 1000000.0 / li.QuadPart;
        }

        QueryPerformanceCounter(&li);

        return static_cast<uint64_t>(li.QuadPart * invFrequency);
#else
        return 0;
#endif
    }

    std::vector<std::string> ARGS;

    void setArgs(const std::vector<std::string>& args)
    {
        ARGS = args;
    }

    const std::vector<std::string>& getArgs()
    {
        return ARGS;
    }

    static std::mt19937 engine(std::random_device{}());

    uint32_t random(uint32_t min, uint32_t max)
    {
        return std::uniform_int_distribution<uint32_t>{min, max}(engine);
    }

    float randomf(float min, float max)
    {
        float diff = max - min;

        float rand = static_cast<float>(engine()) / engine.max();

        return rand * diff + min;
    }
}
