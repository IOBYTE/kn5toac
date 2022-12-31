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
            m_entries.push_back(std::make_pair(first, second));
        }
    }

    fin.close();
}

void lut::dump() const
{
    for (const auto& entry : m_entries)
    {
        std::cout << entry.first << " | " << entry.second << std::endl;
    }
}

float lut::lookup(float first) const
{
    if (m_entries.empty())
        throw std::runtime_error("empty lut");

    if (first < m_entries.front().first)
        return m_entries.front().second;

    if (first > m_entries.back().first)
        return m_entries.back().second;

    for (size_t i = 0; i < m_entries.size() - 1; i++)
    {
        if (first == m_entries[i].first)
            return  m_entries[i].second;

	if (first >= m_entries[i].first && first <= m_entries[i + 1].first)
        {
            const float second_low = m_entries[i].second;
            const float first_low = m_entries[i].first;
            const float first_delta = m_entries[i + 1].first - first_low;
            const float second_delta = m_entries[i + 1].second - second_low;

            if (second_delta == 0)
                return second_low;

            return second_low + ((first - first_low) * second_delta) / first_delta;
        }
    }

    return 0.0f;
}
