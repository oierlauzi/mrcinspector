
#pragma once

#include "DataTypes.h"
#include "Mode.h"

namespace MrcInspector
{

enum class Endianess : Word 
{
    be = 1,    ///< Big endian
    le = 4,    ///< Little endian

    //float, complex, int, char
    be_be_be_be = 0x11110000,
    be_be_be_le = 0x11140000,
    be_be_le_be = 0x11410000,
    be_be_le_le = 0x11440000,
    be_le_be_be = 0x14110000,
    be_le_be_le = 0x14140000,
    be_le_le_be = 0x14410000,
    be_le_le_le = 0x14440000,
    le_be_be_be = 0x41110000,
    le_be_be_le = 0x41140000,
    le_be_le_be = 0x41410000,
    le_be_le_le = 0x41440000,
    le_le_be_be = 0x44110000,
    le_le_be_le = 0x44140000,
    le_le_le_be = 0x44410000,
    le_le_le_le = 0x44440000,
};

template<size_t N>
constexpr Endianess getNibble(Endianess end)
{
    constexpr Word mask = 0x0F;
    constexpr Word shift = 4*N;
    return static_cast<Endianess>((static_cast<Word>(end) >> shift) & mask);
}

constexpr Endianess getCharEndianess(Endianess end)
{
    return getNibble<4>(end);
}

constexpr Endianess getIntEndianess(Endianess end)
{
    return getNibble<5>(end);
}

constexpr Endianess getFloatEndianess(Endianess end)
{
    return getNibble<6>(end);
}

constexpr Endianess getComplexEndianess(Endianess end)
{
    return getNibble<7>(end);
}

constexpr Endianess getModeEndianess(Endianess end, Mode mode)
{
    switch (mode)
    {
    case Mode::sint8:
        return getCharEndianess(end);
    case Mode::sint16:
    case Mode::uint16:
        return getIntEndianess(end);
    case Mode::float32:
    case Mode::float16:
        return getFloatEndianess(end);
    case Mode::cint16:
    case Mode::cfloat32:
        return getComplexEndianess(end);
    default: 
        return static_cast<Endianess>(0); //Not found
    }
}

}