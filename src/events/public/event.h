#pragma once

#include <SDL.h>
#include <cstdint>
#include <type_traits>
#include <variant>

namespace muse
{

/** Enum for event type.*/
enum class EventType : std::uint8_t
{
    KEYBOARD,
    MOUSE,
    MOUSE_BUTTON,
    WHEEL_SCROLL,
    INVALID
};

/** Enum for keystate like if the key is pressed or released. */
enum class KeyState : std::uint8_t
{
    DOWN,
    UP
};

/** Enum for buttonstate, same as key. */
enum class ButtonState : std::uint8_t
{
    DOWN,
    UP
};

/** Enum for all buttons on mouse. */
enum class Button : std::uint8_t
{
    LEFT,
    RIGHT
};

/** Enum for most of keys on keyboard. */
enum class Key : std::uint32_t
{
    NONE = 0,

    NUM_0,        // 0
    NUM_1,        // 1
    NUM_2,        // 2
    NUM_3,        // 3
    NUM_4,        // 4
    NUM_5,        // 5
    NUM_6,        // 6
    NUM_7,        // 7
    NUM_8,        // 8
    NUM_9,        // 9
    A,            // a
    B,            // b
    C,            // c
    D,            // d
    E,            // e
    F,            // f
    G,            // g
    H,            // h
    I,            // i
    J,            // j
    K,            // k
    L,            // l
    M,            // m
    N,            // n
    O,            // o
    P,            // p
    Q,            // q
    R,            // r
    S,            // s
    T,            // t
    U,            // u
    V,            // v
    W,            // w
    X,            // x
    Y,            // y
    Z,            // z
    BACKSPACE,    // backspace
    ENTER,        // enter
    DEL,          // delete
    ESCAPE,       // escape
    LCTRL,        // left ctrl
    RCTRL,        // right ctrl
    UP_ARROW,     // up arrow
    DOWN_ARROW,   // down arrow
    RIGHT_ARROW,  // right arrow
    LEFT_ARROW,   // left arrow
    LALT,         // left alt
    RALT,         // right alt
    LSHIFT,       // left shift
    RSHIFT,       // right shift
    CAPS_LOCK,    // caps lock
    F1,           // f1
    F2,           // f2
    F3,           // f3
    F4,           // f4
    F5,           // f5
    F6,           // f6
    F7,           // f7
    F8,           // f8
    F9,           // f9
    F10,          // f10
    F11,          // f11
    F12,          // f12
    TAB,          // tab
    LESS,         // <
    GREATER,      // >
    EQUALS,       // =
    PLUS,         // +
    MINUS,        // -
    SLASH,        // /
    BACKSLASH,    /* \ */
    SPACE,        // " "
    COLON,        // :
    SEMICOLON,    // ;
    PERIOD,       // .
    COMMA,        // ,
    LBRACE,       // {
    RBRACE,       // }
    LBRACKET,     // [
    RBRACKET,     // ]
    ASTERISK,     // *
    AMPERSAND,    // &
    AT,           // @
    HASHTAG,      // #
    DOLLAR,       // $
    LPAREN,       // (
    RPAREN,       // )
    PERCENT,      // %
    EXCLAIM,      // !
    DOUBLE_QUOTE, // "
    QUOTE         // '
};

/**
 *
 *  This structure doesn't encapsulates any data but allows us
 *  to get/set the state of key in keymap we do this that
 *  the user doesn't need to access some global map
 *  and can access only from this struct that is excepted to be returned
 *  by event class.
 *
 */
struct KeyboardEvent
{
    /**
     *
     *  Create keyboard event.
     *
     *  @param key Key that was pressed or released.
     *  @param state State of the key.
     *
     */
    KeyboardEvent(Key key, KeyState state);

    /**
     *
     *  Get reference to state of key in key map.
     *
     *  @param key Key that is in keymap.
     *
     *  @return State of supplied key.
     *
     */
    KeyState &operator[](Key key) const;

    /** Indicates what's the type of event struct it is (has every event struct). */
    static constexpr EventType type = EventType::KEYBOARD;
};

/**
 *
 *  Structure that encapsulates all of the data
 *  for mouse event like X and Y delta.
 *
 */
struct MouseEvent
{
    /** Delta X*/
    std::int32_t delta_x;

    /** Delta Y. */
    std::int32_t delta_y;

    /** Indicates what's the type of event struct it is (has every event struct). */
    static constexpr EventType type = EventType::MOUSE;
};

/**
 *
 *  Structure that encapsulates all of the data
 *  for mouse button event like what button was pressed or released
 *  and button's state.
 *
 */
struct MouseButtonEvent
{
    /** Button that was pressed or released. */
    Button button;

    /** State of button. */
    ButtonState state;

    /** Indicates what's the type of event struct it is (has every event struct). */
    static constexpr EventType type = EventType::MOUSE_BUTTON;
};

/**
 *
 *  Structure that encapsulates all of data
 *  for wheel scroll event like Y delta of wheel.
 *
 */
struct WheelScrollEvent
{
    /** Delta Y. */
    std::int32_t delta_y;

    /** Indicates what's the type of event struct it is (has every event struct). */
    static constexpr EventType type = EventType::WHEEL_SCROLL;
};

/**
 *
 *  Invalid event, just a placeholder.
 *
 */
struct InvalidEvent
{
    static constexpr EventType type = EventType::INVALID;
};

/**
 *
 *  Type trait to check whether type is event type
 *  or some other type.
 *
 */
template <class>
struct is_event : std::false_type
{
};

template <>
struct is_event<KeyboardEvent> : std::true_type
{
};

template <>
struct is_event<MouseButtonEvent> : std::true_type
{
};

template <>
struct is_event<MouseEvent> : std::true_type
{
};

template <>
struct is_event<WheelScrollEvent> : std::true_type
{
};

template <>
struct is_event<InvalidEvent> : std::true_type
{
};

template <class T>
static constexpr bool is_event_v = is_event<T>::value;

// Concept for is_event type trait
template <class T>
concept IsEvent = is_event_v<T>;

/**
 *
 *  Class that can be briefly described as polymorphic event holder.
 *  It has a std::variant member that stores different types of
 *  event structures and you can get one of them by using get()
 *  function (if event doesn't hold event structure of provided type
 *  exception will be thrown) and you can use type() to get type of stored event structure.
 *
 */
class Event
{
  public:
    /**
     *
     *  Create event.
     *
     *  @param event Event structure.
     *
     */
    template <IsEvent T>
    explicit Event(const T &event)
        : event_(event)
        , type_(T::type)
    {
    }

    /**
     *
     *  Get type of event structure that contains this event.
     *
     *  @return Type.
     *
     */
    EventType type() const
    {
        return type_;
    }

    /**
     *
     *  Get pointer to event structure to get all of store data
     *  like what key was pressed or released, mouse x and y delta, etc.
     *
     *  @return Pointer to struct.
     *
     */
    template <IsEvent T>
    const T &get() const
    {
        return std::get<T>(event_);
    }

  private:
    /** Event variant. */
    std::variant<KeyboardEvent, MouseEvent, MouseButtonEvent, WheelScrollEvent, InvalidEvent> event_;

    /** Type of event. */
    EventType type_;
};

}