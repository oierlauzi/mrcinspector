#pragma once

#include "DataTypes.h"

#include <string_view>

namespace MrcInspector
{

enum class AxisMapping : Word 
{
    x = 1,
    y = 2,
    z = 3
};

constexpr std::string_view toString(AxisMapping x)
{
    switch (x)
    {
    case AxisMapping::x:    return "x";
    case AxisMapping::y:    return "y";
    case AxisMapping::z:    return "z";
    default:                return "";
    }
}

}