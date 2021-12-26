#include <IO.h>

#include <array>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <cassert>

namespace MrcInspector
{

/** STATIC FUNCTIONS **/

static size_t readCharArray(std::istream& is, char* data, size_t size)
{
    return is.readsome(reinterpret_cast<char*>(data), size);
}

static size_t readByteArray(std::istream& is, std::byte* data, size_t size)
{
    return readCharArray(is, reinterpret_cast<char*>(data), size);
}

template<typename T>
static size_t readNative(std::istream& is, T& dst)
{
    //Read directly to the result
    return readByteArray(is, reinterpret_cast<std::byte*>(&dst), sizeof(dst));
}

template<typename T>
static size_t readOtherEndian(std::istream& is, T& dst)
{
    //Read data to a temporary byte array
    std::array<std::byte, sizeof(T)> tmp;
    auto result = readByteArray(is, tmp.data(), tmp.size());

    if(result == tmp.size()) 
    {
        //Swap the endianess (reverse bytes)
        std::reverse(tmp.begin(), tmp.end());

        //Store the result
        std::memcpy(&dst, tmp.data(), tmp.size());
    }
    else 
    {
        result = 0; //We could not read all, so nothing was written
    }

    return result;
}

template<typename T>
static size_t readBigEndian(std::istream& is, T& dst)
{
    #if BYTE_ORDER == BIG_ENDIAN
        return readNative(is, dst);
    #else
        return readOtherEndian(is, dst);
    #endif
}


template<typename T>
static size_t readLittleEndian(std::istream& is, T& dst)
{
    #if BYTE_ORDER == LITTLE_ENDIAN
        return readNative(is, dst);
    #else
        return readOtherEndian(is, dst);
    #endif
}

template<typename Ite>
static size_t readNative(std::istream& is, Ite first, Ite last)
{
    size_t count = 0;
    std::for_each(
        first, last,
        [&is, &count] (auto& el) {count += readNative(is, el);}
    );
    return count;
}

template<typename Ite>
static size_t readOtherEndian(std::istream& is, Ite first, Ite last)
{
    size_t count = 0;
    std::for_each(
        first, last,
        [&is, &count] (auto& el) {count += readOtherEndian(is, el);}
    );
    return count;
}


template<typename Ite>
static size_t readBigEndian(std::istream& is, Ite first, Ite last)
{
    size_t count = 0;
    std::for_each(
        first, last,
        [&is, &count] (auto& el) {count += readBigEndian(is, el);}
    );
    return count;
}

template<typename Ite>
static size_t readLittleEndian(std::istream& is, Ite first, Ite last)
{
    size_t count = 0;
    std::for_each(
        first, last,
        [&is, &count] (auto& el) {count += readLittleEndian(is, el);}
    );
    return count;
}

template <typename T>
static size_t readDataImpl(std::istream& is, const MainHeader& header, std::vector<T>& data)
{
    size_t result = 0;

    //Check that the requested type matches the type provided by the header
    assert(header.mode == DataTypeMode<T>::value);
    
    //Obtain the size of the volume
    const auto nElements = std::accumulate(
        header.dimensions.cbegin(), header.dimensions.cend(),
        1, std::multiplies<>()
    );
    data.resize(nElements);

    // Read according to the endianess
    if(getModeEndianess(header.byteOrder, header.mode) == Endianess::be)
    {
        result = readBigEndian(is, data.begin(), data.end());
    }
    else if(getModeEndianess(header.byteOrder, header.mode) == Endianess::le)
    {
        result = readLittleEndian(is, data.begin(), data.end());
    }
    
    return result;
}

template <typename T>
static size_t readDataImpl(std::istream& is, const MainHeader& header, DataVariant& data)
{
    data.emplace<std::vector<T>>();
    return readDataImpl(is, header, std::get<std::vector<T>>(data));
}

/** PUBLIC FUNCTIONS **/

size_t readMainHeader(std::istream& is, MainHeader& header)
{
    size_t count = 0;
    count += readBigEndian(is, header.dimensions.begin(), header.dimensions.end());
    count += readBigEndian(is, header.mode);
    count += readBigEndian(is, header.start.begin(), header.start.end());
    count += readBigEndian(is, header.sampling.begin(), header.sampling.end());
    count += readBigEndian(is, header.cellDimensions.begin(), header.cellDimensions.end());
    count += readBigEndian(is, header.cellAngles.begin(), header.cellAngles.end());
    count += readBigEndian(is, header.axisMapping.begin(), header.axisMapping.end());
    count += readBigEndian(is, header.min);
    count += readBigEndian(is, header.max);
    count += readBigEndian(is, header.avg);
    count += readBigEndian(is, header.ispg);
    count += readBigEndian(is, header.extHeaderLen);
    count += readBigEndian(is, header.extHeaderType);
    count += readBigEndian(is, header.version);
    count += readBigEndian(is, header.padding.begin(), header.padding.end());
    count += readBigEndian(is, header.origin.begin(), header.origin.end());
    count += readCharArray(is, header.map.data(), header.map.size());
    count += readBigEndian(is, header.byteOrder);
    count += readBigEndian(is, header.rms);
    count += readBigEndian(is, header.nLabels);
    count += readCharArray(is, header.labels.front().data(), header.labels.size()*sizeof(Label));
    return count;
}

size_t readExtendedHeader(std::istream& is, const MainHeader& header, std::string& extHeader)
{
    extHeader.resize(header.extHeaderLen);
    return readCharArray(is, extHeader.data(), extHeader.size());
}

size_t readData(std::istream& is, const MainHeader& header, DataVariant& data)
{
    size_t result = 0;
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