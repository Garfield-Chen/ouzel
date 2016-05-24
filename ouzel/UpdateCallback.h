// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Egnine;

    class UpdateCallback: public Noncopyable, public ReferenceCounted
    {
        friend Engine;
    public:
        std::function<void(float)> callback;
    };
}
