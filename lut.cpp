#include "lut.h"

#include <iostream>
#include <fstream>

void lut::read(const std::string& fileName)
{
    std::ifstream fin(fileName);

    if (!fin)
        throw std::runtime_error("Couldn't open file: " + fileName);

    std::string line;

    while (std::getline(fin, line))
    {
        if (!line.empty())
        {
            const size_t seperator = line.find('|');
            const float rpm = std::stof(line.substr(0, seperator));
            const float tq = std::stof(line.substr(seperator + 1));
            entries.push_back(std::make_pair(rpm, tq));
        }
    }

    fin.close();
}

void lut::dump() const
{
    for (const auto& entry : entries)
    {
        std::cout << entry.first << " | " << entry.second << std::endl;
    }
}
