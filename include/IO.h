#pragma once

#include "MainHeader.h"

#include <istream>
#include <string>
#include <vector>

namespace MrcInspector
{

size_t readMainHeader(std::istream& is, MainHeader& header);
size_t readExtendedHeader(std::istream& is, const MainHeader& header, std::string& extHeader);
size_t readData(std::istream& is, const MainHeader& header, DataVariant& data);

}