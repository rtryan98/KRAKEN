#pragma once
#include "KRAKEN/core/event/Event.h"

namespace kraken
{
    class AppUpdateEvent : public Event
    {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE( AppUpdate );
        EVENT_CLASS_CATEGORY( EVENT_CATEGORY_APPLICATION );
    };

    class AppRenderEvent : public Event
    {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender);
        EVENT_CLASS_CATEGORY( EVENT_CATEGORY_APPLICATION );
    };

    class AppTickEvent : public Event
    {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE( AppTick );
        EVENT_CLASS_CATEGORY( EVENT_CATEGORY_APPLICATION );
    };
}
