// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "utils/Types.h"

#ifdef OPENAL
#undef OPENAL
#endif

namespace ouzel
{
    class Engine;

    namespace audio
    {
        class Audio
        {
            friend Engine;
        public:
            enum class Driver
            {
                DEFAULT,
                NONE,
                OPENAL,
                XAUDIO2,
                OPENSL
            };

            virtual ~Audio();
            virtual void free();

            virtual bool init();

            virtual SoundDataPtr createSoundData();
            virtual SoundPtr createSound();

            bool isReady() const { return ready; }

        protected:
            Audio(Driver pDriver = Driver::NONE);

            Driver driver;

            bool ready = false;
        };
    } // namespace audio
} // namespace ouzel
