// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include "Noncopyable.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "FileSystem.h"
#include "Input.h"
#include "EventDispatcher.h"
#include "UpdateCallback.h"
#include "Settings.h"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    extern Engine* sharedEngine;

    class Cache;
    class Localization;

    class Engine: public Noncopyable
    {
    public:
        Engine();
        virtual ~Engine();

        static std::set<graphics::Renderer::Driver> getAvailableDrivers();

        bool init(Settings& newSettings);
        const Settings& getSettings() const { return settings; }

        EventDispatcher* getEventDispatcher() const { return eventDispatcher; }
        Cache* getCache() const { return cache; }
        Window* getWindow() const { return window; }
        graphics::Renderer* getRenderer() const { return renderer; }
        scene::SceneManager* getSceneManager() const { return sceneManager; }
        FileSystem* getFileSystem() const { return fileSystem; }
        input::Input* getInput() const { return input; }
        Localization* getLocalization() const { return localization; }

        void exit();

        void begin();
        void end();
        bool run();

        float getTargetFPS() const { return targetFPS; }
        float getFPS() const { return currentFPS; }

        void scheduleUpdate(UpdateCallback* callback);
        void unscheduleUpdate(UpdateCallback* callback);

    protected:
        void lock();
        void unlock();

        Settings settings;
        
        EventDispatcher* eventDispatcher = nullptr;
        input::Input* input = nullptr;
        Cache* cache = nullptr;
        Window* window = nullptr;
        FileSystem* fileSystem = nullptr;
        Localization* localization = nullptr;
        graphics::Renderer* renderer = nullptr;
        scene::SceneManager* sceneManager = nullptr;

        float targetFPS;
        float currentFPS = 0.0f;
        uint64_t previousFrameTime;

        std::vector<UpdateCallback*> updateCallbacks;
        std::set<UpdateCallback*> updateCallbackAddList;
        std::set<UpdateCallback*> updateCallbackRemoveList;

        int32_t locked = 0;
        bool running = false;
        bool active = true;
    };
}
