#include <Read.h>
#include <Print.h>

#include <fstream>
#include <iostream>

using namespace MrcInspector;

static void readAll(std::istream& is, MainHeader& header, std::string& extHeader, DataBlock& data)
{
    size_t count;

    //Read the header
    count = readMainHeader(is, header);
    if(count != sizeof(MainHeader))
    {
        std::cerr << "Error reading header. Expected 1024B. Read " << count << "B" << std::endl;
        std::terminate();
    }
    
    //Read the extended header
    count = readExtendedHeader(is, header, extHeader);
    if(count != extHeader.size())
    {
        std::cerr << "Error reading the extended header. Expected " << extHeader.size() << "B. Read " << count << "B" << std::endl;
        std::terminate();
    }

    //Read the data block
    count = readData(is, header, data);
    std::visit(
        [count] (const auto& x)
        {
            const auto size = sizeof(typename std::decay<decltype(x)>::type::value_type) * x.size();
            if(size != count)
            {
                std::cerr << "Error reading the data block. Expected " << size << "B. Read " << count << "B" << std::endl;
                std::terminate();
            }
        },
        data
    );
}

static void printAll(std::ostream& os, const MainHeader& header, const std::string& extHeader, const DataBlock& data)
{
    os << "==================== HEADER ====================\n";
    printHeader(os, header);
    os << "================ EXTENDED HEADER ===============\n";
    os << extHeader << '\n';
    os << "================== DATA BLOCK ==================\n";
    printData(os, header, data);
}

int main(int argc, const char* argv[]) {
    if(argc < 2) 
    {
        std::cerr << "Usage: " << argv[0] << " [mrc file]" << std::endl;
        std::terminate();
    }

    // Open input file
    std::ifstream file(argv[1]);

    //Read from file
    MainHeader header;
    std::string extHeader;
    DataBlock data;
    readAll(file, header, extHeader, data);

    //Print all to stdout
    printAll(std::cout, header, extHeader, data);

    return 0;
}