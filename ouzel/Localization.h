// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <string>
#include "Noncopyable.h"

namespace ouzel
{
    class Language;

    class Localization: public Noncopyable
    {
    public:
        ~Localization();

        void addLanguage(const std::string& name, const std::string& filename);
        void setLanguage(const std::string& language);
        std::string getString(const std::string& str);

    protected:
        std::map<std::string, Language*> languages;
        Language* currentLanguage = nullptr;
    };
}
