#include <Read.h>

#include <array>
#include <algorithm>
#include <numeric>
#include <type_traits>
#include <cstring>
#include <cassert>

namespace MrcInspector
{

/** STATIC FUNCTIONS **/

template<typename T>
static void swapEndianess(T& value)
{
    static_assert(std::is_scalar<T>::value, "Type must be a scalar type");

    //Swap its endianess in place (reverse bytes)
    auto* first = reinterpret_cast<std::byte*>(&value);
    auto* last = first + sizeof(T);
    std::reverse(first, last);
}

template<typename T>
static void swapEndianess(std::complex<T>& value)
{
    //Swap the endianess for each of the components
    swapEndianess(reinterpret_cast<T*>(&value)[0]);
    swapEndianess(reinterpret_cast<T*>(&value)[1]);
}

static void swapEndianess(half_float::half& value)
{
    swapEndianess(reinterpret_cast<uint16&>(value));
}

template<typename T>
static void makeBigEndian(T& value)
{
    #if BYTE_ORDER == LITTLE_ENDIAN
        swapEndianess(value);
    #else
        (void)value; //No-op
    #endif
}

template<typename T>
static void makeLittleEndian(T& value)
{
    #if BYTE_ORDER == BIG_ENDIAN
        swapEndianess(value);
    #else
        (void)value; //No-op
    #endif
}

template<typename T>
using MakeEndianFunc = void (*)(T&);

template<typename T>
static MakeEndianFunc<T> getMakeEndianFunc(Endianess endianess)
{
    switch (endianess)
    {
    case Endianess::be: return makeBigEndian<T>;
    case Endianess::le: return makeLittleEndian<T>;
    default:            return nullptr;
    }
}

template <typename T>
static size_t readDataImpl(std::istream& is, const MainHeader& header, std::vector<T>& data)
{
    //Check that the requested type matches the type provided by the header
    assert(header.mode == DataTypeMode<T>::value);
    
    //Obtain the size of the volume
    const auto nElements = std::accumulate(
        header.dimensions.cbegin(), header.dimensions.cend(),
        1U, std::multiplies<>()
    );
    const auto nBytes = nElements * sizeof(T);
    data.resize(nElements);

    //Read from disk
    size_t result = 0;
    is.read(reinterpret_cast<char*>(data.data()), nBytes);
    if(is.good())
    {
        //Match the endianess
        const auto makeEndianessFunc = getMakeEndianFunc<T>(getModeEndianess(header.byteOrder, DataTypeMode<T>::value));
        if(makeEndianessFunc)
        {
            std::for_each(
                data.begin(), data.end(),
                makeEndianessFunc
            );
            result = nBytes;
        }
    }
    
    return result;
}

template <typename T>
static size_t readDataImpl(std::istream& is, const MainHeader& header, DataBlock& data)
{
    data.emplace<std::vector<T>>();
    return readDataImpl(is, header, std::get<std::vector<T>>(data));
}

/** PUBLIC FUNCTIONS **/

size_t readMainHeader(std::istream& is, MainHeader& header)
{
    //Read the whole file
    size_t result = 0;
    is.read(reinterpret_cast<char*>(&header), sizeof(header));
    if(is.good())
    {
        //Endianess and exttype is always stored as BE. Ensure it is correctly stored
        makeBigEndian(header.byteOrder);
        makeBigEndian(header.extHeaderType);

        //Get the functions for ensuring endinaness
        const auto makeEndianFuncInt = getMakeEndianFunc<uint32>(getIntEndianess(header.byteOrder));
        const auto makeEndianFuncFlt = getMakeEndianFunc<float32>(getFloatEndianess(header.byteOrder));

        //Match the endianess
        if(makeEndianFuncInt && makeEndianFuncFlt)
        {
            //Integers
            makeEndianFuncInt(header.dimensions[0]);
            makeEndianFuncInt(header.dimensions[1]);
            makeEndianFuncInt(header.dimensions[2]);
            makeEndianFuncInt(reinterpret_cast<uint32&>(header.mode));
            makeEndianFuncInt(header.start[0]);
            makeEndianFuncInt(header.start[1]);
            makeEndianFuncInt(header.start[2]);
            makeEndianFuncInt(header.sampling[0]);
            makeEndianFuncInt(header.sampling[1]);
            makeEndianFuncInt(header.sampling[2]);
            makeEndianFuncInt(reinterpret_cast<uint32&>(header.axisMapping[0]));
            makeEndianFuncInt(reinterpret_cast<uint32&>(header.axisMapping[1]));
            makeEndianFuncInt(reinterpret_cast<uint32&>(header.axisMapping[2]));
            makeEndianFuncInt(header.ispg);
            makeEndianFuncInt(header.extHeaderLen);
            makeEndianFuncInt(header.version);
            makeEndianFuncInt(header.nLabels);

            //Floats
            makeEndianFuncFlt(header.cellDimensions[0]);
            makeEndianFuncFlt(header.cellDimensions[1]);
            makeEndianFuncFlt(header.cellDimensions[2]);
            makeEndianFuncFlt(header.cellAngles[0]);
            makeEndianFuncFlt(header.cellAngles[1]);
            makeEndianFuncFlt(header.cellAngles[2]);
            makeEndianFuncFlt(header.min);
            makeEndianFuncFlt(header.max);
            makeEndianFuncFlt(header.avg);
            makeEndianFuncFlt(header.origin[0]);
            makeEndianFuncFlt(header.origin[1]);
            makeEndianFuncFlt(header.origin[2]);
            makeEndianFuncFlt(header.rms);

            //All OK
            result = sizeof(MainHeader);
        }
    }

    return result;
}

size_t readExtendedHeader(std::istream& is, const MainHeader& header, std::string& extHeader)
{
    extHeader.resize(header.extHeaderLen);
    is.read(extHeader.data(), extHeader.size());
    return is.good() ? extHeader.size() : 0;
}

size_t readData(std::istream& is, const MainHeader& header, DataBlock& data)
{
    size_t result;
    switch (header.mode)
    {
    case Mode::sint8:       result = readDataImpl<int8>(is, header, data); break;
    case Mode::sint16:      result = readDataImpl<int16>(is, header, data); break;
    case Mode::float32:     result = readDataImpl<float32>(is, header, data); break;
    case Mode::cint16:      result = readDataImpl<cint32>(is, header, data); break;
    case Mode::cfloat32:    result = readDataImpl<cfloat64>(is, header, data); break;
    case Mode::uint16:      result = readDataImpl<uint16>(is, header, data); break;
    case Mode::float16:     result = readDataImpl<float16>(is, header, data); break;
    default:                result = 0; break;
    }

    return result;
}

}