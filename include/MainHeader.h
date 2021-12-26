#pragma once

#include "DataTypes.h"
#include "Mode.h"
#include "AxisMapping.h"
#include "ExtendedHeaderType.h"
#include "Endianess.h"

#include <array>

namespace MrcInspector
{

struct MainHeader
{
    std::array<uint32, 3>       dimensions;     ///< Dimensions (x, y, z)
    Mode                        mode;           ///< Data storage mode
    std::array<uint32, 3>       start;          ///< Location of the first axis in unit cells (x, y, z)
    std::array<uint32, 3>       sampling;       ///< Sampling along each axis (x, y, z)
    std::array<float32, 3>      cellDimensions; ///< Cell dimensions on each axis (x, y, z) [A]
    std::array<float32, 3>      cellAngles;     ///< Cell angles on each axis (x, y, z) [deg]
    std::array<AxisMapping, 3>  axisMapping;    ///< Axis corresponding to row, columns and sections
    float32                     min;            ///< Minimum density value
    float32                     max;            ///< Maximum density value
    float32                     avg;            ///< Average density value
    uint32                      ispg;           ///< Space group number
    uint32                      extHeaderLen;   ///< Size of the extended header in bytes
    ExtendedHeaderType          extHeaderType;  ///< Type of the extended header
    uint32                      version;        ///< Version of the MRC file
    std::array<uint32, 23>      padding;        ///< Padding for extra data
    std::array<float32, 3>      origin;         ///< Phase origin [A]
    std::array<char, 4>         map;            ///< String 'MAP'
    Endianess                   byteOrder;      ///< Endianess of the data
    float32                     rms;            ///< RMS deviation of the density value
    uint32                      nLabels;        ///< Number of labels used
    std::array<Label, 10>       labels;         ///< Labels
};

static_assert(sizeof(MainHeader) == 1024, "Size of the header file does not match the expected size (1024B)");

}