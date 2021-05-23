#pragma once

#include "app/key_codes.h"

#include <bitset>
#include <vector>

namespace vrender
{
enum class EventType
{
    KeyPress,
    KeyRelease,
    KeyType,
    WindowClose,
    WindowResize,
    WindowMove,
    MouseButtonPress,
    MouseButtonRelease,
    MouseMove,
    MouseScroll,
    InputState
};

class Event;

class EventHandler
{
public:
    EventHandler() {}
    virtual void handle(const Event& event) = 0;

    EventHandler(const EventHandler&) = delete;
    void operator=(const EventHandler&) = delete;
};

class Event
{
public:
    virtual EventType type() const = 0;
};

class WindowResizeEvent : public Event
{

public:
    WindowResizeEvent(unsigned int width, unsigned int height) : m_Width(width), m_Height(height) {}

    unsigned int width() const { return m_Width; }
    unsigned int height() const { return m_Height; }

    virtual EventType type() const override { return EventType::WindowResize; }

private:
    unsigned int m_Width;
    unsigned int m_Height;
};

class InputStateEvent : public Event
{
public:
    InputStateEvent(std::bitset<NUM_KEYBOARD_KEYS> keyStates) : m_KeyStates(keyStates) {}

    virtual EventType type() const override { return EventType::InputState; }

    std::bitset<NUM_KEYBOARD_KEYS> keyStates() const { return m_KeyStates; }

private:
    std::bitset<NUM_KEYBOARD_KEYS> m_KeyStates;
};

}; // namespace vrender
