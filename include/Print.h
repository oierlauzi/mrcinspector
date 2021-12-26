#pragma once

#include "MainHeader.h"

#include <ostream>
#include <string>
#include <vector>

namespace MrcInspector
{

void printHeader(std::ostream& os, const MainHeader& header);
void printData(std::ostream& os, const MainHeader& header, const DataBlock& data);

}