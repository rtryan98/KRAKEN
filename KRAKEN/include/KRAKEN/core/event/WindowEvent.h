#pragma once
#include "KRAKEN/core/event/Event.h"
#include "KRAKEN/Types.h"

namespace kraken
{
    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : width{ width }, height{ height }
        {}

        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }

        EVENT_CLASS_TYPE( WindowResize );
        EVENT_CLASS_CATEGORY( EVENT_CATEGORY_WINDOW );
    private:
        uint32_t width;
        uint32_t height;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose);
        EVENT_CLASS_CATEGORY(EVENT_CATEGORY_WINDOW);
    };
}
