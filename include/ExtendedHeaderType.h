#pragma once

#include "DataTypes.h"

#include <array>
#include <string_view>

namespace MrcInspector
{

constexpr Word makeExtendedHeaderCode(char c0, char c1, char c2, char c3) 
{
    Word result = 0;
    result |= c0 << 24;
    result |= c1 << 16;
    result |= c2 << 8;
    result |= c3 << 0;
    return result;
}

enum class ExtendedHeaderType : Word 
{
    ccp4 = makeExtendedHeaderCode('C', 'C', 'C', '4'),
    mrco = makeExtendedHeaderCode('M', 'R', 'C', 'O'),
    seri = makeExtendedHeaderCode('S', 'E', 'R', 'I'),
    agar = makeExtendedHeaderCode('A', 'G', 'A', 'R'),
    fei1 = makeExtendedHeaderCode('F', 'E', 'I', '1'),
    fei2 = makeExtendedHeaderCode('F', 'E', 'I', '2'),
    hdf5 = makeExtendedHeaderCode('H', 'D', 'F', '5'),
};

constexpr std::string_view toString(ExtendedHeaderType x)
{
    switch (x)
    {
    case ExtendedHeaderType::ccp4:  return "CCP4";
    case ExtendedHeaderType::mrco:  return "MRCO";
    case ExtendedHeaderType::seri:  return "SERI";
    case ExtendedHeaderType::agar:  return "AGAR";
    case ExtendedHeaderType::fei1:  return "FEI1";
    case ExtendedHeaderType::fei2:  return "FEI2";
    case ExtendedHeaderType::hdf5:  return "HDF5";
    default:                        return "";
    }
}

}