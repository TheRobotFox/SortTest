#ifndef OS_H_
#define OS_H_

#include <cstddef>
#include <cstdint>
#include <string>

namespace UI {

    struct OS
    {
        virtual void sleep_while_active(size_t ms) = 0;
    };


    enum class KeyCmd : uint8_t {
        NONE, SPEED_UP, SPEED_DOWN, TOGGLE_PAUSE
    };

    struct Input
    {
        virtual auto get_key() -> KeyCmd = 0;
    };

    struct Rect
    {
        int left,top,right,bottom;
        auto operator==(const Rect& other) const -> bool = default;
    };

    struct Color
    {
        unsigned char r,g,b;
    };

    struct Draw
    {
        virtual auto get_screen_dimensions() -> Rect = 0;

        virtual void draw_begin() = 0;
        virtual void draw_rect(Rect r, Color c) = 0;
        virtual void draw_end() = 0;
        virtual void draw_text(Rect pos, std::string text) = 0;
    };

}

#endif // OS_H_
