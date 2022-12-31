#ifndef _LUI_H_
#define _LUT_H_

#include <string>
#include <vector>
#include <utility>

class lut
{
private:
    std::vector<std::pair<float,float>> m_entries;

public:
    lut() = default;
    explicit lut(const std::string& fileName)
    {
        read(fileName);
    }

    void read(const std::string& fileName);
    void dump() const;
    const std::vector<std::pair<float, float>>& getValues() const
    {
        return m_entries;
    }
    float lookup(float first) const;
};

#endif
