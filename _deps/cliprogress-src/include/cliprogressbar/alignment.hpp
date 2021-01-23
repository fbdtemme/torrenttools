#pragma once
#include <cstdint>
#include <type_traits>

namespace cliprogress {

enum class alignment : std::uint8_t
{
    left               = 0x01,     ///< snap to left, leaving space on right or bottom
    center             = 0x02,     ///< center natural width of widget inside the allocation
    right              = 0x04      ///< snap to right, leaving space on left or top
};

}
