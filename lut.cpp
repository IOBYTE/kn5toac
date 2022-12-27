#include "lut.h"
#include "trim.h"

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
        std::string trimmed = trim(line);

        if (!trimmed.empty() && trimmed[0] != ';')
        {
            const size_t seperator = trimmed.find('|');
            const float first = std::stof(trimmed.substr(0, seperator));
            const float second = std::stof(trimmed.substr(seperator + 1));
            entries.push_back(std::make_pair(first, second));
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
