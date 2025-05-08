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

    struct Config
    {
        float ops_per_second=20;
        int mergedistance=10;
        float resolution_scale=1.0;
    };

}
