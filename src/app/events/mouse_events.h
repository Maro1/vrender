#pragma once

#include "events.h"

namespace vrender
{

class MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(int x, int y) : m_X(x), m_Y(y) {}

    inline int x() const { return m_X; }
    inline int y() const { return m_Y; }

    virtual EventType type() const override { return EventType::MouseMove; }

private:
    int m_X, m_Y;
};

class MousePressedEvent : public Event
{
public:
    MousePressedEvent(int button, int x, int y, int mods) : m_Button(button), m_X(x), m_Y(y), m_Mods(mods) {}

    virtual EventType type() const override { return EventType::MouseButtonPress; }
    inline int button() const { return m_Button; }
    inline int x() const { return m_X; }
    inline int y() const { return m_X; }
    inline int mods() const { return m_Mods; }

private:
    int m_Button;
    int m_X, m_Y;
    int m_Mods;
};

class MouseReleasedEvent : public Event
{
public:
    MouseReleasedEvent(int button, int x, int y) : m_Button(button), m_X(x), m_Y(y) {}

    virtual EventType type() const override { return EventType::MouseButtonRelease; }
    inline int button() const { return m_Button; }
    inline int x() const { return m_X; }
    inline int y() const { return m_Y; }

private:
    int m_Button;
    int m_X, m_Y;
};

class MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(int xOffset, int yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

    virtual EventType type() const override { return EventType::MouseScroll; }
    inline int offsetX() const { return m_XOffset; }
    inline int offsetY() const { return m_YOffset; }

private:
    int m_XOffset, m_YOffset;
};
}; // namespace vrender
