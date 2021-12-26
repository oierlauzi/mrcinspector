#include <IO.h>

#include <fstream>
#include <iostream>

using namespace MrcInspector;

static void readAll(std::istream& is, MainHeader& header, std::string& extHeader, DataVariant& data)
{
    size_t count;

    //Read the header
    count = readMainHeader(is, header);
    if(count != 1024)
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
    //TODO check result
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
    DataVariant data;
    readAll(file, header, extHeader, data);


}