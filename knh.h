#ifndef _KNH_H_
#define _KNH_H_

#include "kn5.h"

#include <iostream>
#include <string>
#include <vector>

class knh
{
    struct Entry
    {
        std::string         m_name;
        kn5::Matrix         m_matrix;
        std::vector<Entry>  m_children;

        void read(std::istream& stream);
        void dump(std::ostream& stream, const std::string& indent = "") const;
    };

    Entry   m_entry;

public:
    knh() = default;
    explicit knh(const std::string& fileName)
    {
        read(fileName);
    }

    void read(const std::string& fileName);
    void dump(std::ostream& stream) const;
    bool dump(const std::string& fileName) const;
};

#endif
