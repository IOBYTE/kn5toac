#include "ini.h"

#include <iostream>
#include <fstream>

static const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
    const size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
    const size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}

void ini::read(const std::string& fileName)
{
    std::ifstream stream(fileName, std::ios::binary);

    if (!stream)
        throw std::runtime_error("Couldn't open file");

    char        line[256];
    std::string currentSection;

    while (stream.getline(line, sizeof(line)))
    {
        std::string trimmed = trim(line);

        if (trimmed.empty())
            continue;

        if (trimmed[0] == ';')
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
                throw std::runtime_error("Couldn't parse file");

            const std::string key = rtrim(trimmed.substr(0, equal));
            const std::string value = ltrim(trimmed.substr(equal + 1));

            sections[currentSection].insert(std::pair(key, value));
        }
    }
}

void ini::dump() const
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
