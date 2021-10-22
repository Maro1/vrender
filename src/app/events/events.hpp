#pragma once

#include "app/key_codes.hpp"
#include "utils/noncopyable.hpp"

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

class EventHandler : private NonCopyable
{
public:
    EventHandler() = default;
    virtual ~EventHandler() = default;

    virtual void handle(const Event& event) = 0;
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

    inline unsigned int width() const { return m_Width; }
    inline unsigned int height() const { return m_Height; }

    virtual EventType type() const override { return EventType::WindowResize; }

private:
    unsigned int m_Width;
    unsigned int m_Height;
};

class InputStateEvent : public Event
{
public:
    InputStateEvent(std::bitset<NUM_KEYBOARD_KEYS> keyStates, double deltaTime)
        : m_KeyStates(keyStates), m_DeltaTime(deltaTime)
    {
    }

    virtual EventType type() const override { return EventType::InputState; }

    inline std::bitset<NUM_KEYBOARD_KEYS> keyStates() const { return m_KeyStates; }
    inline double deltaTime() const { return m_DeltaTime; }

private:
    std::bitset<NUM_KEYBOARD_KEYS> m_KeyStates;
    double m_DeltaTime;
};

}; // namespace vrender
