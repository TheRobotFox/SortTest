#pragma once

#include <cstdint>
namespace UI {

    enum class KeyCmd : uint8_t {
        NONE, SPEED_UP, SPEED_DOWN, TOGGLE_PAUSE
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
    const Color grey {.r = 128, .g = 128, .b = 128};
    const Color red {.r = 255, .g = 20, .b = 20};
    const Color green {.r = 20, .g = 255, .b = 20};
    const Color blue {.r = 20, .g = 20, .b = 200};
    const Color white {.r = 255, .g = 255, .b = 255};
    const Color black {.r=0, .g = 0, .b = 0};

    struct Config
    {
        float ops_per_second=20;
        int mergedistance=10;
        float resolution_scale=1.0;
    };

}
