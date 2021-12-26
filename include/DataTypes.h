#pragma once

#include "half/half.hpp"

#include <cstdint>
#include <array>
#include <complex>
#include <variant>
#include <vector>

namespace MrcInspector
{

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;

using float16 = half_float::half; static_assert(sizeof(float16) == 2, "Unexpected size of float16");
using float32 = float; static_assert(sizeof(float32) == 4, "Unexpected size of float32");

using cint32 = std::complex<int16>;
using cint64 = std::complex<int32>;
using cfloat32 = std::complex<float16>;
using cfloat64 = std::complex<float32>;

using Word = int32;
using Label = std::array<char, 80>;

using DataBlock = std::variant<
    std::vector<int8>,
    std::vector<int16>,
    std::vector<float32>,
    std::vector<cint32>,
    std::vector<cfloat64>,
    std::vector<uint16>,
    std::vector<float16>
    >;

}