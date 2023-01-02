#include "ini.h"
#include "trim.h"

#include <iostream>
#include <fstream>
#include <sstream>

void ini::read(const std::string& fileName)
{
    std::ifstream stream(fileName, std::ios::binary);

    if (!stream)
        throw std::runtime_error("Couldn't open file: " + fileName);

    std::string line;
    size_t      lineNumber = 0;
    std::string currentSection;

    while (std::getline(stream, line))
    {
        lineNumber++;

        std::string trimmed = trim(line);

        if (trimmed.empty())
            continue;

        // comment
        if (trimmed[0] == ';' || trimmed[0] == '#' || trimmed[0] == '/')
            continue;

        if (trimmed[0] == '[')
        {
            const size_t closeBracket = trimmed.find(']');

            if (closeBracket == std::string::npos)
                throw std::runtime_error("Couldn't parse file");

            currentSection = trimmed.substr(1, closeBracket - 1);
        }
        else
        {
            const size_t equal = trimmed.find_first_of('=');

            if (equal == std::string::npos)
            {
                continue;
                // throw std::runtime_error("Couldn't parse file: " + fileName + " line : " + std::to_string(lineNumber));
            }

            const std::string key = rtrim(trimmed.substr(0, equal));
            std::string value = ltrim(trimmed.substr(equal + 1));
            const size_t comment = value.find(';');
            if (comment != std::string::npos)
                value = rtrim(value.substr(0, comment));

            sections[currentSection].insert(std::pair(key, value));
        }
    }

    stream.close();
}

void ini::dump(std::ostream& stream) const
{
    for (const auto& section : sections)
    {
        std::cout << "[" << section.first << "]" << std::endl;

        for (const auto& entry : section.second)
        {
            std::cout << entry.first << " = " << entry.second << std::endl;
        }
    }
}

bool ini::dump(const std::string& fileName) const
{
    std::ofstream	of(fileName);

    if (of)
    {
        dump(of);

        of.close();

        return true;
    }

    return false;
}

std::string ini::getValue(const std::string& section, const std::string& key) const
{
    std::string value;

    auto it = sections.find(section);

    if (it != sections.end())
    {
        auto it2 = it->second.find(key);

        if (it2 != it->second.end())
            value = it2->second;
    }

    return value;
}

int ini::getIntValue(const std::string& section, const std::string& key) const
{
    return std::stoi(getValue(section, key));
}

float ini::getFloatValue(const std::string& section, const std::string& key) const
{
    return std::stof(getValue(section, key));
}

std::array<float, 3> ini::getFloatArray3Value(const std::string& section, const std::string& key) const
{
    std::array<float, 3>    vec3{ 0, 0, 0 };
    std::istringstream ss(getValue(section, key));
    std::string number;
    size_t index = 0;

    while (index < 3 && std::getline(ss, number, ','))
        vec3[index++] = std::stof(number);

    return vec3;
}

bool ini::hasSection(const std::string& section) const
{
    return sections.find(section) != sections.end();
}
