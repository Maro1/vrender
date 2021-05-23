#pragma once

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
    MouseScroll
};

class Event;

class EventHandler
{
public:
    virtual void handle(const Event& event) = 0;
};

class Event
{
public:
    virtual EventType getType() const = 0;
};

class WindowResizeEvent : public Event
{

public:
    WindowResizeEvent(unsigned int width, unsigned int height) : m_Width(width), m_Height(height) {}

    unsigned int width() const { return m_Width; }
    unsigned int height() const { return m_Height; }

    virtual EventType getType() const override { return EventType::WindowResize; }

private:
    unsigned int m_Width;
    unsigned int m_Height;
};

}; // namespace vrender
