// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "File.h"
#include "Engine.h"
#include "FileSystem.h"
#include "Utils.h"

namespace ouzel
{
    File::File(const std::string& filename, Mode mode, bool binary)
    {
        std::string modeStr;

        switch (mode)
        {
            case Mode::READ:
                modeStr = "r";
                break;
            case Mode::WRITE:
                modeStr = "w";
                break;
            case Mode::APPEND:
                modeStr = "a";
                break;
        }

        if (binary)
        {
            modeStr += "b";
        }

        file = fopen(sharedEngine->getFileSystem()->getPath(filename).c_str(), modeStr.c_str());

        if (!file)
        {
            log("Failed to open file %s", filename.c_str());
        }
    }

    File::~File()
    {
        if (file)
        {
            fclose(file);
        }
    }

    int64_t File::read(char* buffer, uint32_t size)
    {
        return static_cast<int64_t>(fread(buffer, size, 1, file));
    }

    int64_t File::write(const char* buffer, uint32_t size)
    {
        return static_cast<int64_t>(fwrite(buffer, size, 1, file));
    }
}
