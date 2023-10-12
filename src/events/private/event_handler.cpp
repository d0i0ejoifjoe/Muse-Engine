#include "events/public/event_handler.h"

muse::Key process_key(SDL_Keycode keycode)
{
    muse::Key key;

    switch (keycode)
    {
        case SDLK_0: key = muse::Key::NUM_0; break;
        case SDLK_1: key = muse::Key::NUM_1; break;
        case SDLK_2: key = muse::Key::NUM_2; break;
        case SDLK_3: key = muse::Key::NUM_3; break;
        case SDLK_4: key = muse::Key::NUM_4; break;
        case SDLK_5: key = muse::Key::NUM_5; break;
        case SDLK_6: key = muse::Key::NUM_6; break;
        case SDLK_7: key = muse::Key::NUM_7; break;
        case SDLK_8: key = muse::Key::NUM_8; break;
        case SDLK_9: key = muse::Key::NUM_9; break;
        case SDLK_a: key = muse::Key::A; break;
        case SDLK_b: key = muse::Key::B; break;
        case SDLK_c: key = muse::Key::C; break;
        case SDLK_d: key = muse::Key::D; break;
        case SDLK_e: key = muse::Key::E; break;
        case SDLK_f: key = muse::Key::F; break;
        case SDLK_g: key = muse::Key::G; break;
        case SDLK_h: key = muse::Key::H; break;
        case SDLK_i: key = muse::Key::I; break;
        case SDLK_j: key = muse::Key::J; break;
        case SDLK_k: key = muse::Key::K; break;
        case SDLK_l: key = muse::Key::L; break;
        case SDLK_m: key = muse::Key::M; break;
        case SDLK_n: key = muse::Key::N; break;
        case SDLK_o: key = muse::Key::O; break;
        case SDLK_p: key = muse::Key::P; break;
        case SDLK_q: key = muse::Key::Q; break;
        case SDLK_r: key = muse::Key::R; break;
        case SDLK_s: key = muse::Key::S; break;
        case SDLK_t: key = muse::Key::T; break;
        case SDLK_u: key = muse::Key::U; break;
        case SDLK_v: key = muse::Key::V; break;
        case SDLK_w: key = muse::Key::W; break;
        case SDLK_x: key = muse::Key::X; break;
        case SDLK_y: key = muse::Key::Y; break;
        case SDLK_z: key = muse::Key::Z; break;
        case SDLK_BACKSPACE: key = muse::Key::BACKSPACE; break;
        case SDLK_RETURN: key = muse::Key::ENTER; break;
        case SDLK_DELETE: key = muse::Key::DEL; break;
        case SDLK_ESCAPE: key = muse::Key::ESCAPE; break;
        case SDLK_LCTRL: key = muse::Key::LCTRL; break;
        case SDLK_RCTRL: key = muse::Key::RCTRL; break;
        case SDLK_UP: key = muse::Key::UP_ARROW; break;
        case SDLK_DOWN: key = muse::Key::DOWN_ARROW; break;
        case SDLK_RIGHT: key = muse::Key::RIGHT_ARROW; break;
        case SDLK_LEFT: key = muse::Key::LEFT_ARROW; break;
        case SDLK_LALT: key = muse::Key::LALT; break;
        case SDLK_RALT: key = muse::Key::RALT; break;
        case SDLK_LSHIFT: key = muse::Key::LSHIFT; break;
        case SDLK_RSHIFT: key = muse::Key::RSHIFT; break;
        case SDLK_CAPSLOCK: key = muse::Key::CAPS_LOCK; break;
        case SDLK_F1: key = muse::Key::F1; break;
        case SDLK_F2: key = muse::Key::F2; break;
        case SDLK_F3: key = muse::Key::F3; break;
        case SDLK_F4: key = muse::Key::F4; break;
        case SDLK_F5: key = muse::Key::F5; break;
        case SDLK_F6: key = muse::Key::F6; break;
        case SDLK_F7: key = muse::Key::F7; break;
        case SDLK_F8: key = muse::Key::F8; break;
        case SDLK_F9: key = muse::Key::F9; break;
        case SDLK_F10: key = muse::Key::F10; break;
        case SDLK_F11: key = muse::Key::F11; break;
        case SDLK_F12: key = muse::Key::F12; break;
        case SDLK_TAB: key = muse::Key::TAB; break;
        case SDLK_LESS: key = muse::Key::LESS; break;
        case SDLK_GREATER: key = muse::Key::GREATER; break;
        case SDLK_EQUALS: key = muse::Key::EQUALS; break;
        case SDLK_PLUS: key = muse::Key::PLUS; break;
        case SDLK_MINUS: key = muse::Key::MINUS; break;
        case SDLK_SLASH: key = muse::Key::SLASH; break;
        case SDLK_BACKSLASH: key = muse::Key::BACKSLASH; break;
        case SDLK_SPACE: key = muse::Key::SPACE; break;
        case SDLK_COLON: key = muse::Key::COLON; break;
        case SDLK_SEMICOLON: key = muse::Key::SEMICOLON; break;
        case SDLK_PERIOD: key = muse::Key::PERIOD; break;
        case SDLK_COMMA: key = muse::Key::COMMA; break;
        case SDLK_LEFTBRACKET: key = muse::Key::LBRACKET; break;
        case SDLK_RIGHTBRACKET: key = muse::Key::RBRACKET; break;
        case SDLK_ASTERISK: key = muse::Key::ASTERISK; break;
        case SDLK_AMPERSAND: key = muse::Key::AMPERSAND; break;
        case SDLK_AT: key = muse::Key::AT; break;
        case SDLK_HASH: key = muse::Key::HASHTAG; break;
        case SDLK_DOLLAR: key = muse::Key::DOLLAR; break;
        case SDLK_LEFTPAREN: key = muse::Key::LPAREN; break;
        case SDLK_RIGHTPAREN: key = muse::Key::RPAREN; break;
        case SDLK_PERCENT: key = muse::Key::PERCENT; break;
        case SDLK_EXCLAIM: key = muse::Key::EXCLAIM; break;
        case SDLK_QUOTEDBL: key = muse::Key::DOUBLE_QUOTE; break;
        case SDLK_QUOTE: key = muse::Key::QUOTE; break;
        default: key = muse::Key::NONE; break;
    }

    return key;
}

muse::Event process_event(const SDL_Event *e)
{
    switch (e->type)
    {
        case SDL_KEYDOWN: return muse::Event{muse::KeyboardEvent{process_key(e->key.keysym.sym), muse::KeyState::DOWN}};
        case SDL_KEYUP: return muse::Event{muse::KeyboardEvent{process_key(e->key.keysym.sym), muse::KeyState::UP}};
        case SDL_MOUSEMOTION: return muse::Event{muse::MouseEvent{e->motion.xrel, e->motion.yrel}};
        case SDL_MOUSEBUTTONDOWN:
            switch (e->button.button)
            {
                case SDL_BUTTON_LEFT: return muse::Event{muse::MouseButtonEvent{muse::Button::LEFT, muse::ButtonState::DOWN}};
                case SDL_BUTTON_RIGHT: return muse::Event{muse::MouseButtonEvent{muse::Button::RIGHT, muse::ButtonState::DOWN}};
                default: return muse::Event{muse::InvalidEvent{}};
            }
        case SDL_MOUSEBUTTONUP:
            switch (e->button.button)
            {
                case SDL_BUTTON_LEFT: return muse::Event{muse::MouseButtonEvent{muse::Button::LEFT, muse::ButtonState::UP}};
                case SDL_BUTTON_RIGHT: return muse::Event{muse::MouseButtonEvent{muse::Button::RIGHT, muse::ButtonState::UP}};
                default: return muse::Event{muse::InvalidEvent{}};
            }
        case SDL_MOUSEWHEEL: return muse::Event{muse::WheelScrollEvent{e->wheel.y}};
        default: return muse::Event{muse::InvalidEvent{}};
    }

    return muse::Event{muse::InvalidEvent{}};
}

namespace muse
{

EventHandler::EventHandler()
    : callbacks_()
{
}

void EventHandler::dispatch()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        auto event = process_event(&e);

        for (const auto &callback : callbacks_)
        {
            if (!callback(event)) // if function return false that means that user doesn't want to event propagate down the hierarchy
            {
                break;
            }
        }
    }
}

void EventHandler::add_callback(EventCallback callback)
{
    callbacks_.push_back(callback);
}

void EventHandler::remove_callback(std::uint32_t index)
{
    callbacks_.erase(std::begin(callbacks_) + index);
}
}