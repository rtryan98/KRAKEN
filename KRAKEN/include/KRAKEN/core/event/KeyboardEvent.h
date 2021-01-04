#pragma once
#include "KRAKEN/core/event/Event.h"
#include "KRAKEN/Types.h"

namespace kraken
{
    class KeyEvent : public Event
    {
    public:
        uint32_t getKeyCode() const { return keyCode; }
        EVENT_CLASS_CATEGORY( EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT );

    protected:
        KeyEvent(uint32_t keyCode)
            : keyCode{ keyCode }
        {}

    private:
        uint32_t keyCode;
    };

    class KeyPressEvent : public KeyEvent
    {
    public:
        KeyPressEvent(uint32_t keyCode)
            : KeyEvent( keyCode )
        {}
        EVENT_CLASS_TYPE( KeyPress );
    };

    class KeyReleaseEvent : public KeyEvent
    {
    public:
        KeyReleaseEvent(uint32_t keyCode)
            : KeyEvent( keyCode )
        {}
        EVENT_CLASS_TYPE( KeyRelease );
    };

    class KeyRepeatEvent : public KeyEvent
    {
    public:
        KeyRepeatEvent(uint32_t keyCode, uint32_t repeatCount)
            : KeyEvent( keyCode ), repeatCount{ repeatCount }
        {}
        EVENT_CLASS_TYPE( KeyRepeat );

    private:
        uint32_t repeatCount;
    };
}