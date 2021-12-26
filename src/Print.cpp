
#include <Print.h>

#include <algorithm>
#include <iomanip>

namespace MrcInspector
{

/** STATIC FUNCTIONS **/

void printName(std::ostream& os, std::string_view name)
{
    os << std::setw(24) << name << ": ";
}

template <typename T>
static void printNum(std::ostream& os, std::string_view name, T value)
{
    printName(os, name);
    os << std::dec << value << '\n';
}

static void printText(std::ostream& os, std::string_view name, std::string_view text)
{
    printName(os, name);
    os << text << '\n';
}

template <typename T>
static void printEnum(std::ostream& os, std::string_view name, T value)
{
    printName(os, name);
    os << std::dec << static_cast<int>(value) << " (" << toString(value) << ")\n";
}

static std::string_view toStringBeLe(Endianess x)
{
    switch(x)
    {
        case Endianess::be: return "be";
        case Endianess::le: return "le";
        default:            return "";
    }
}

static void printByteOrder(std::ostream& os, std::string_view name, Endianess value)
{
    printName(os, name);
    os << std::hex << static_cast<Word>(value) << " (";
    os << "char: " << toStringBeLe(getCharEndianess(value));
    os << ", int: " << toStringBeLe(getIntEndianess(value));
    os << ", float: " << toStringBeLe(getFloatEndianess(value));
    os << ", complex: " << toStringBeLe(getComplexEndianess(value));
    os << ")\n";
}

template <typename T> 
static void printDataImpl(std::ostream& os, const MainHeader& header, const std::vector<T>& data)
{
    const auto& dimensions = header.dimensions;
    const std::array<size_t, 3> strides = {
        1,
        dimensions[0],
        dimensions[0]*dimensions[1]
    };

    for(size_t s = 0; s < dimensions[2]; ++s)
    {
        for(size_t r = 0; r < dimensions[1]; ++r)
        {
            for(size_t c = 0; c < dimensions[0]; ++c)
            {
                size_t i = strides[0]*c + strides[1]*r + strides[2]*s;
                os << std::setw(12) << data[i];
            }
            os << '\n';
        }
        os << "\n\n\n";
    }
}

/** PUBLIC FUNCTIONS **/

void printHeader(std::ostream& os, const MainHeader& header)
{
    printNum(os, "Columns", header.dimensions[0]);
    printNum(os, "Rows", header.dimensions[1]);
    printNum(os, "Sections", header.dimensions[2]);
    printEnum(os, "Mode", header.mode);
    printNum(os, "Column start", header.start[0]);
    printNum(os, "Row start", header.start[1]);
    printNum(os, "Section start", header.start[2]);
    printNum(os, "X sampling", header.sampling[0]);
    printNum(os, "Y sampling", header.sampling[1]);
    printNum(os, "Z sampling", header.sampling[2]);
    printNum(os, "X cell dimension", header.cellDimensions[0]);
    printNum(os, "Y cell dimension", header.cellDimensions[1]);
    printNum(os, "Z cell dimension", header.cellDimensions[2]);
    printNum(os, "Alpha cell angle", header.cellAngles[0]);
    printNum(os, "Beta cell angle", header.cellAngles[1]);
    printNum(os, "Gamma cell angle", header.cellAngles[2]);
    printEnum(os, "Column mapping", header.axisMapping[0]);
    printEnum(os, "Row mapping", header.axisMapping[1]);
    printEnum(os, "Section mapping", header.axisMapping[2]);
    printNum(os, "Min density", header.min);
    printNum(os, "Max density", header.max);
    printNum(os, "Avg density", header.avg);
    printNum(os, "Space group number", header.ispg);
    printNum(os, "Extended header length", header.extHeaderLen);
    printEnum(os, "Extended header type", header.extHeaderType);
    printNum(os, "Version", header.version);
    printNum(os, "X origin", header.origin[0]);
    printNum(os, "Y origin", header.origin[1]);
    printNum(os, "Z origin", header.origin[2]);
    printText(os, "Map", std::string_view(header.map.data(), header.map.size()));
    printByteOrder(os, "Byte order", header.byteOrder);
    printNum(os, "RMS density", header.rms);
    printNum(os, "Label count", header.nLabels);
    for (size_t i = 0; i < header.labels.size(); ++i)
    {
        std::string name = "Label #" + std::to_string(i);
        printText(os, name, std::string_view(header.labels[i].data(), header.labels[i].size()));
    }
}

void printData(std::ostream& os, const MainHeader& header, const DataBlock& data)
{
    std::visit(
        [&os, &header] (const auto& values) {
            printDataImpl(os, header, values);
        },
        data
    );
}

}