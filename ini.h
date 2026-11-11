#ifndef _INI_H_
#define _INI_H_

#include <string>
#include <map>
#include <array>
#include <iosfwd>
#include <stdexcept>

class ini
{
private:
    std::string                                               m_fileName;
    std::map<std::string, std::map<std::string, std::string>> m_sections;

    std::runtime_error valueError(const std::string& section, const std::string& key, const std::string& value) const;

public:
    ini() = default;
    explicit ini(const std::string& fileName)
    {
        read(fileName);
    }

    void read(const std::string& fileName);
    // Reads fileName if it exists. Returns false (leaving the ini empty but
    // remembering the name for messages) if it doesn't.
    bool readIfExists(const std::string& fileName);
    // The get*Value functions throw std::runtime_error naming the file, section
    // and key when a value is missing or can't be parsed.
    void dump(std::ostream& stream) const;
    bool dump(const std::string& fileName) const;
    std::string getValue(const std::string& section, const std::string& key) const;
    int getIntValue(const std::string& section, const std::string& key) const;
    float getFloatValue(const std::string& section, const std::string& key) const;
    float getFloatValue(const std::string& section, const std::string& key, const float value) const;
    std::array<float,3> getFloatArray3Value(const std::string& section, const std::string& key) const;
    bool hasSection(const std::string& section) const;
    bool hasValue(const std::string& section, const std::string& key) const;
    const std::string& getFileName() const
    {
        return m_fileName;
    }
    bool hasSections() const
    {
        return m_sections.size() > 0;
    }
};

#endif

