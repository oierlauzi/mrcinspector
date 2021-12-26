#pragma once

#include "DataTypes.h"

namespace MrcInspector
{

enum class Mode : Word {
    sint8       = 0,    ///< 8bit signed integers
    sint16      = 1,    ///< 16bit signed integers
    float32     = 2,    ///< 32bit IEEE-754
    cint16      = 3,    ///< 2x 16bit signed integers as a cartesian complex representation
    cfloat32    = 4,    ///< 2x 32bit IEEE-754 as a cartesian complex representation
    uint16      = 6,    ///< 16bit unsigned integers
    float16     = 12,   ///< 16bit IEEE-754
};

template<typename T>
struct DataTypeMode
{
};

template<>
struct DataTypeMode<int8>
{
    static constexpr Mode value = Mode::sint8;
};

template<>
struct DataTypeMode<int16>
{
    static constexpr Mode value = Mode::sint16;
};

template<>
struct DataTypeMode<float32>
{
    static constexpr Mode value = Mode::float32;
};

template<>
struct DataTypeMode<cint32>
{
    static constexpr Mode value = Mode::cint16;
};

template<>
struct DataTypeMode<cfloat64>
{
    static constexpr Mode value = Mode::cfloat32;
};

template<>
struct DataTypeMode<uint16>
{
    static constexpr Mode value = Mode::uint16;
};

template<>
struct DataTypeMode<float16>
{
    static constexpr Mode value = Mode::float16;
};

constexpr std::string_view toString(Mode x)
{
    switch (x)
    {
    case Mode::sint8:   return "signed int8";
    case Mode::sint16:  return "signed int16";
    case Mode::float32: return "float32";
    case Mode::cint16:  return "complex int16";
    case Mode::cfloat32:return "complex float32";
    case Mode::uint16:  return "unsigned int16";
    case Mode::float16: return "float16";
    default:            return "";
    }
}

}