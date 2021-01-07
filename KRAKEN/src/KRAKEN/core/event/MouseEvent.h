#pragma once
#include "KRAKEN/core/event/Event.h"
#include "KRAKEN/Types.h"

namespace kraken
{
    class MouseButtonEvent : public Event
    {
    public:
        uint32_t getButton() const { return button; }
        EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_MOUSE_BUTTON | EVENT_CATEGORY_INPUT);
    
    protected:
        MouseButtonEvent(uint32_t button)
            : button{ button }
        {}

    private:
        uint32_t button;
    };

    class MouseButtonPressEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressEvent(uint32_t button)
            : MouseButtonEvent( button )
        {}

        EVENT_CLASS_TYPE( MouseButtonPress );
    };

    class MouseButtonReleaseEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleaseEvent(uint32_t button)
            : MouseButtonEvent( button )
        {}

        EVENT_CLASS_TYPE( MouseButtonRelease );
    };

    class MouseMoveEvent : public Event
    {
    public:
        MouseMoveEvent(double_t x, double_t y)
            : x{ x }, y{ y }
        {}

        double_t getX() const { return x; };
        double_t getY() const { return y; };

        EVENT_CLASS_TYPE( MouseMove );
        EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT);

    private:
        double_t x;
        double_t y;
    };

    class MouseScrollEvent : public Event
    {
    public:
        MouseScrollEvent(double_t xOffset, double_t yOffset)
            : xOffset{ xOffset }, yOffset{ yOffset }
        {}

        double_t getXOffset() const { return xOffset; };
        double_t getYOffset() const { return yOffset; };

        EVENT_CLASS_TYPE( MouseScroll );
        EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT);

    private:
        double_t xOffset;
        double_t yOffset;
    };
}
