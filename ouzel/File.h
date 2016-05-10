// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <cstdio>
#include <cstdint>
#include "Noncopyable.h"

namespace ouzel
{
    class FileSystem;

    class File: public Noncopyable
    {
    public:
        enum class Mode
        {
            READ,
            WRITE,
            APPEND
        };

        File(const std::string& filename, Mode mode, bool binary);
        virtual ~File();

        operator bool() const
        {
            return file != nullptr;
        }

        bool isOpen() const
        {
            return file != nullptr;
        }

        FILE* getFile() const { return file; }

        int64_t read(char* buffer, uint32_t size);
        int64_t write(const char* buffer, uint32_t size);

    protected:
        FILE* file = nullptr;
    };
}
