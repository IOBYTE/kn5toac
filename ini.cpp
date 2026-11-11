#include "ini.h"
#include "trim.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

void ini::read(const std::string& fileName)
{
    m_fileName = fileName;

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

            m_sections[currentSection].insert(std::pair(key, value));
        }
    }

    stream.close();
}

void ini::dump(std::ostream& stream) const
{
    stream << "fileName: " << m_fileName << std::endl;

    for (const auto& section : m_sections)
    {
        stream << "[" << section.first << "]" << std::endl;

        for (const auto& entry : section.second)
            stream << entry.first << " = " << entry.second << std::endl;
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

bool ini::hasValue(const std::string& section, const std::string& key) const
{
    auto it = m_sections.find(section);

    if (it != m_sections.end())
    {
        auto it2 = it->second.find(key);

        if (it2 != it->second.end())
            return true;
    }

    return false;
}

std::string ini::getValue(const std::string& section, const std::string& key) const
{
    std::string value;

    auto it = m_sections.find(section);

    if (it != m_sections.end())
    {
        auto it2 = it->second.find(key);

        if (it2 != it->second.end())
            value = it2->second;
    }

    return value;
}

// std::stoi/std::stof throw std::invalid_argument / std::out_of_range, which
// are logic_errors, not runtime_errors. Convert them to a runtime_error that
// says which file, section and key was missing or bad.
std::runtime_error ini::valueError(const std::string& section, const std::string& key, const std::string& value) const
{
    const std::string where = (m_fileName.empty() ? std::string("ini") : m_fileName) + ": [" + section + "] " + key;

    if (!hasValue(section, key))
        return std::runtime_error(where + " is missing");

    return std::runtime_error(where + " has invalid value '" + value + "'");
}

int ini::getIntValue(const std::string& section, const std::string& key) const
{
    const std::string value = getValue(section, key);

    try
    {
        return std::stoi(value);
    }
    catch (const std::logic_error&)
    {
        throw valueError(section, key, value);
    }
}

float ini::getFloatValue(const std::string& section, const std::string& key) const
{
    const std::string value = getValue(section, key);

    try
    {
        return std::stof(value);
    }
    catch (const std::logic_error&)
    {
        throw valueError(section, key, value);
    }
}

float ini::getFloatValue(const std::string& section, const std::string& key, const float value) const
{
    if (!hasValue(section, key))
        return value;

    return getFloatValue(section, key);
}

std::array<float, 3> ini::getFloatArray3Value(const std::string& section, const std::string& key) const
{
    std::array<float, 3>    vec3{ 0, 0, 0 };
    const std::string value = getValue(section, key);
    std::istringstream ss(value);
    std::string number;
    size_t index = 0;

    try
    {
        while (index < 3 && std::getline(ss, number, ','))
            vec3[index++] = std::stof(number);
    }
    catch (const std::logic_error&)
    {
        throw valueError(section, key, value);
    }

    return vec3;
}

bool ini::readIfExists(const std::string& fileName)
{
    m_fileName = fileName;

    std::error_code ec;

    if (!std::filesystem::is_regular_file(fileName, ec))
        return false;

    read(fileName);

    return true;
}

bool ini::hasSection(const std::string& section) const
{
    return m_sections.find(section) != m_sections.end();
}
