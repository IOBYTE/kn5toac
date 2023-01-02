#ifndef _ACD_H_
#define _ACD_H_

#include <string>
#include <vector>
#include <list>
#include <filesystem>

class acd
{
    struct Entry
    {
        std::string         m_name;
        std::vector<int>    m_rawData;
        std::string         m_data;

        void dump(std::ostream& stream) const;
    };

    std::list<Entry>        m_entries;
    std::string             m_key;

    void calculateKey(const std::string& directory);

public:
    acd() = default;
    explicit acd(const std::string& fileName)
    {
        read(fileName);
    }

    void read(const std::string& fileName);
    void writeEntries(const std::string& directory) const;
    void dump(std::ostream& stream) const;
    bool dump(const std::string& fileName) const;
};

#endif
