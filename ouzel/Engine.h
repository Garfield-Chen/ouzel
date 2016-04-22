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
#include "App.h"

void ouzelMain(const std::vector<std::string>& args);

namespace ouzel
{
    extern const EnginePtr sharedEngine;

    class Engine: public Noncopyable
    {
    public:
        Engine();
        virtual ~Engine();

        std::set<graphics::Renderer::Driver> getAvailableDrivers() const;

        bool init(Settings& settings);

        EventDispatcher* getEventDispatcher() const { return _eventDispatcher; }
        Cache* getCache() const { return _cache; }
        Window* getWindow() const { return _window; }
        graphics::Renderer* getRenderer() const { return _renderer; }
        scene::SceneManager* getSceneManager() const { return _sceneManager; }
        FileSystem* getFileSystem() const { return _fileSystem; }
        input::Input* getInput() const { return _input; }
        Localization* getLocalization() const { return _localization; }

        void exit();

        void begin();
        void end();
        bool run();

        float getTargetFPS() const { return _targetFPS; }
        float getFPS() const { return _currentFPS; }

        void scheduleUpdate(const UpdateCallbackPtr& callback);
        void unscheduleUpdate(const UpdateCallbackPtr& callback);

        void setApp(const AppPtr& app);
        const AppPtr& getApp() const { return _app; }

    protected:
        void lock();
        void unlock();

        AppPtr _app;

        EventDispatcher* _eventDispatcher = nullptr;
        Cache* _cache = nullptr;
        Window* _window = nullptr;
        graphics::Renderer* _renderer = nullptr;
        scene::SceneManager* _sceneManager = nullptr;
        FileSystem* _fileSystem = nullptr;
        input::Input* _input = nullptr;
        Localization* _localization = nullptr;

        float _targetFPS;
        float _currentFPS = 0.0f;
        uint64_t _previousFrameTime;

        std::vector<UpdateCallback*> _updateCallbacks;
        std::set<UpdateCallback*> _updateCallbackAddList;
        std::set<UpdateCallback*> _updateCallbackRemoveList;

        int32_t _locked = 0;
        bool _running = false;
        bool _active = true;
    };
}
