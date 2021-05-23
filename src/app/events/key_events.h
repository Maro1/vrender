#pragma once

#include "events.h"

namespace vrender
{

class KeyEvent : public Event
{
public:
    int getKeyCode() const { return m_Keycode; }

protected:
    int m_Keycode;
    KeyEvent(int keycode) : m_Keycode(keycode) {}
};

class KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

    virtual EventType type() const override { return EventType::KeyPress; }

private:
    int m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

    virtual EventType type() const override { return EventType::KeyRelease; }
};

class KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

    virtual EventType type() const override { return EventType::KeyType; }
};
}; // namespace vrender
