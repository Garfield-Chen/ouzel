// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cassert>

namespace ouzel
{
    class ReferenceCounted
    {
    public:
        ReferenceCounted()
        {
            //LeakHunter::addObject(this);
        }

        virtual ~ReferenceCounted()
        {
            //LeakHunter::removeObject(this);
        }

        void retain() const { ++referenceCounter; }

        bool release() const
        {
            assert(referenceCounter > 0 && "Reference count must be positive");

            if (--referenceCounter <= 0)
            {
                delete this;
                return true;
            }

            return false;
        }

        int32_t getReferenceCount() const
        {
            return referenceCounter;
        }

        const char* getDebugName() const
        {
            return debugName;
        }

    protected:
        void setDebugName(const char* newName)
        {
            debugName = newName;
        }

    private:
        const char* debugName = nullptr;
        mutable int32_t referenceCounter = 1;
    };
}
