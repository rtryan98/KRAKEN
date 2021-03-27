#pragma once

namespace ygg
{
    enum EventCategory
    {
        EVENT_CATEGORY_NONE = 0,
        EVENT_CATEGORY_APPLICATION = 1,
        EVENT_CATEGORY_INPUT = 2,
        EVENT_CATEGORY_KEYBOARD = 4,
        EVENT_CATEGORY_MOUSE = 8,
        EVENT_CATEGORY_MOUSE_BUTTON = 16,
        EVENT_CATEGORY_WINDOW = 32
    };

    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize,
        AppTick, AppUpdate, AppRender,
        KeyPress, KeyRelease, KeyRepeat,
        MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll
    };

    /// Some Macros so creating events is less copied code
    /// This isn't really beautiful but it saves a lot of typing

    #define EVENT_CLASS_CATEGORY(category)                                      \
    virtual int getCategoryFlags() const override { return category; }
    
    #define EVENT_CLASS_TYPE(type)                                                      \
    static EventType getStaticEventType() { return EventType::type; }                   \
    virtual EventType getEventType() const override { return getStaticEventType(); }    \
    virtual const char* getDebugName() const override { return #type; }

    /// End Event Macro defines

    class Event
    {
    private:
        friend class EventDispatcher;
    public:
        bool isHandled = false;
        virtual ~Event() = default;
        virtual EventType getEventType() const = 0;
        virtual int getCategoryFlags() const = 0;
        virtual const char* getDebugName() const = 0;
        inline bool isInCategory(EventCategory category) const { return getCategoryFlags() & category; };
    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event) : event(event) {}
        template<typename T, typename F> bool dispatch(const F& function)
        {
            if (event.getEventType() == T::getStaticEventType())
            {
                event.isHandled = function(static_cast<T&>(event));
                return true;
            }
            return false;
        }
    private:
        Event& event;
    };

}
