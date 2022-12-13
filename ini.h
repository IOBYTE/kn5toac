#ifndef _INI_H_
#define _INI_H_

#include <string>
#include <map>

class ini
{
private:
    std::map<std::string, std::map<std::string, std::string>> sections;

public:
    ini() = default;

    void read(const std::string& fileName);
    void dump() const;
    std::string getValue(const std::string& section, const std::string& key) const;
};

#endif

