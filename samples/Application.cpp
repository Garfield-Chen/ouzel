// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Application.h"
#include <cmath>

using namespace std;
using namespace ouzel;

Application::~Application()
{
    sharedEngine->getEventDispatcher()->removeEventHandler(eventHandler);
}

void Application::begin()
{
    sharedEngine->getLocalization()->addLanguage("latvian", "lv.mo");
    sharedEngine->getLocalization()->setLanguage("latvian");

    eventHandler = new EventHandler();

    eventHandler->keyboardHandler = std::bind(&Application::handleKeyboard, this, std::placeholders::_1, std::placeholders::_2);
    eventHandler->mouseHandler = std::bind(&Application::handleMouse, this, std::placeholders::_1, std::placeholders::_2);
    eventHandler->touchHandler = std::bind(&Application::handleTouch, this, std::placeholders::_1, std::placeholders::_2);
    eventHandler->gamepadHandler = std::bind(&Application::handleGamepad, this, std::placeholders::_1, std::placeholders::_2);
    eventHandler->uiHandler = std::bind(&Application::handleUI, this, std::placeholders::_1, std::placeholders::_2);

    sharedEngine->getEventDispatcher()->addEventHandler(eventHandler);

    sharedEngine->getRenderer()->setClearColor(graphics::Color(64, 0, 0));
    sharedEngine->getWindow()->setTitle("Sample");

    scene::Scene* scene = new scene::Scene();
    sharedEngine->getSceneManager()->setScene(scene);

    rtLayer = new scene::Layer();
    scene->addLayer(rtLayer);
    renderTarget = ouzel::sharedEngine->getRenderer()->createRenderTarget(Size2(256.0f, 256.0f), false);
    rtLayer->setCamera(new scene::Camera());
    rtLayer->setRenderTarget(renderTarget);

    layer = new scene::Layer();
    scene->addLayer(layer);
    layer->setCamera(new scene::Camera());

    uiLayer = new scene::Layer();
    scene->addLayer(uiLayer);
    uiLayer->setCamera(new scene::Camera());

    scene::DebugDrawable* debugDrawable = new scene::DebugDrawable();
    debugDrawable->rectangle(Rectangle(100.0f, 100.0f), graphics::Color(0, 128, 128, 255), true);
    debugDrawable->rectangle(Rectangle(100.0f, 100.0f), graphics::Color(255, 255, 255, 255), false);
    debugDrawable->line(Vector2(0.0f, 0.0f), Vector2(50.0f, 50.0f), graphics::Color(0, 255, 255, 255));
    debugDrawable->point(Vector2(75.0f, 75.0f), graphics::Color(255, 0, 0, 255));

    debugDrawable->circle(Vector2(75.0f, 75.0f), 20.0f, graphics::Color(0, 0, 255, 255));
    debugDrawable->circle(Vector2(25.0f, 75.0f), 20.0f, graphics::Color(0, 0, 255, 255), true);

    scene::Node* drawNode = new scene::Node();
    drawNode->addDrawable(debugDrawable);
    drawNode->setPosition(Vector2(-300, 0.0f));
    layer->addChild(drawNode);

    drawNode->animate(new ouzel::scene::Shake(10.0f, Vector2(10.0f, 20.0f), 20.0f));

    scene::Sprite* characterSprite = scene::Sprite::createFromFile("run.json");
    characterSprite->play(true);

    character = new scene::Node();
    character->addDrawable(characterSprite);
    layer->addChild(character);
    character->setPosition(Vector2(-300.0f, 0.0f));

    std::vector<scene::Animator*> sequence = {
        new scene::Move(4.0f, Vector2(300.0f, 0.0f)),
        new scene::Fade(2.0f, 0.4f)
    };

    character->animate(new scene::Sequence(sequence));

    scene::Sprite* fireSprite = scene::Sprite::createFromFile("fire.json");
    fireSprite->play(true);

    scene::Node* fireNode = new scene::Node();
    fireNode->addDrawable(fireSprite);
    fireNode->setPosition(Vector2(-100.0f, -100.0f));
    layer->addChild(fireNode);
    fireNode->animate(new scene::Fade(5.0f, 0.5f));

    scene::ParticleSystem* flameParticleSystem = scene::ParticleSystem::createFromFile("flame.json");

    flame = new scene::Node();
    flame->addDrawable(flameParticleSystem);
    layer->addChild(flame);

    scene::Sprite* witchSprite = scene::Sprite::createFromFile("witch.png");
    //witchSprite->setColor(graphics::Color(128, 0, 255, 255));

    witch = new scene::Node();
    witch->addDrawable(witchSprite);
    witch->setPosition(Vector2(100.0f, 100.0f));
    layer->addChild(witch);
    witch->animate(new scene::Repeat(new scene::Rotate(1.0f, TAU, false), 3));

    gui::Label* label = gui::Label::create("font.fnt", sharedEngine->getLocalization()->getString("Test"));
    uiLayer->addChild(label);

    std::vector<scene::Animator*> sequence2 = {
        new scene::Animator(1.0f), // delay
        new scene::Ease(new scene::Move(2.0f, Vector2(0.0f, -240.0f), false), scene::Ease::Type::OUT, scene::Ease::Func::BOUNCE)
    };

    label->animate(new scene::Sequence(sequence2));

    button = gui::Button::create("button.png", "button.png", "button_down.png", "", "", graphics::Color(), "");
    button->setPosition(Vector2(-200.0f, 200.0f));
    uiLayer->addChild(button);

    // Render target

    ouzel::scene::Node* rtCharacter = new scene::Node();
    rtCharacter->addDrawable(characterSprite);
    rtLayer->addChild(rtCharacter);

    ouzel::scene::SpriteFrame* rtFrame = ouzel::scene::SpriteFrame::create(Rectangle(0.0f, 0.0f, 256.0f, 256.0f), renderTarget->getTexture(), false, renderTarget->getTexture()->getSize(), Vector2(), Vector2(0.5f, 0.5f));

    ouzel::scene::Sprite* rtSprite = ouzel::scene::Sprite::createFromSpriteFrames({ rtFrame });
    ouzel::scene::Node* rtNode = new ouzel::scene::Node();
    rtNode->addDrawable(rtSprite);
    rtNode->setPosition(Vector2(200.0f, 200.0f));
    layer->addChild(rtNode);

    sharedEngine->getInput()->startGamepadDiscovery();
}

bool Application::handleKeyboard(KeyboardEvent* event, void* sender) const
{
    OUZEL_UNUSED(sender);

    if (event->type == Event::Type::KEY_DOWN)
    {
        Vector2 position = layer->getCamera()->getPosition();

        switch (event->key)
        {
            case input::KeyboardKey::UP:
                position.y += 10.0f;
                break;
            case input::KeyboardKey::DOWN:
                position.y -= 10.0f;
                break;
            case input::KeyboardKey::LEFT:
                position.x -= 10.0f;
                break;
            case input::KeyboardKey::RIGHT:
                position.x += 10.0f;
                break;
            case input::KeyboardKey::SPACE:
                witch->setVisible(!witch->isVisible());
                break;
            case input::KeyboardKey::RETURN:
                sharedEngine->getWindow()->setSize(Size2(640.0f, 480.0f));
                break;
            case input::KeyboardKey::TAB:
                button->setEnabled(!button->isEnabled());
                break;
            default:
                break;
        }

        layer->getCamera()->setPosition(position);
    }

    return true;
}

bool Application::handleMouse(MouseEvent* event, void* sender) const
{
    OUZEL_UNUSED(sender);

    switch (event->type)
    {
        case Event::Type::MOUSE_DOWN:
        {
            sharedEngine->getInput()->setCursorVisible(!sharedEngine->getInput()->isCursorVisible());
            break;
        }
        case Event::Type::MOUSE_MOVE:
        {
            Vector2 worldLocation = layer->getCamera()->convertScreenToWorld(event->position);
            flame->setPosition(worldLocation);
            break;
        }
        default:
            break;
    }

    return true;
}

bool Application::handleTouch(TouchEvent* event, void* sender) const
{
    OUZEL_UNUSED(sender);

    Vector2 worldLocation = layer->getCamera()->convertScreenToWorld(event->position);
    flame->setPosition(worldLocation);

    return true;
}

bool Application::handleGamepad(GamepadEvent* event, void* sender) const
{
    OUZEL_UNUSED(sender);

    if (event->type == Event::Type::GAMEPAD_BUTTON_CHANGE)
    {
        Vector2 position = layer->getCamera()->convertWorldToScreen(flame->getPosition());

        switch (event->button)
        {
            case input::GamepadButton::DPAD_UP:
            case input::GamepadButton::LEFT_THUMB_UP:
            case input::GamepadButton::RIGHT_THUMB_UP:
                position.y = event->value;
                break;
            case input::GamepadButton::DPAD_DOWN:
            case input::GamepadButton::LEFT_THUMB_DOWN:
            case input::GamepadButton::RIGHT_THUMB_DOWN:
                position.y = -event->value;
                break;
            case input::GamepadButton::DPAD_LEFT:
            case input::GamepadButton::LEFT_THUMB_LEFT:
            case input::GamepadButton::RIGHT_THUMB_LEFT:
                position.x = -event->value;
                break;
            case input::GamepadButton::DPAD_RIGHT:
            case input::GamepadButton::LEFT_THUMB_RIGHT:
            case input::GamepadButton::RIGHT_THUMB_RIGHT:
                position.x = event->value;
                break;
            case input::GamepadButton::A:
                witch->setVisible(!event->pressed);
                break;
            default:
                break;
        }

        Vector2 worldLocation = layer->getCamera()->convertScreenToWorld(position);
        flame->setPosition(worldLocation);
    }

    return true;
}

bool Application::handleUI(UIEvent* event, void* sender) const
{
    if (event->type == Event::Type::UI_CLICK_NODE && sender == button)
    {
        character->setVisible(!character->isVisible());
    }

    return true;
}
