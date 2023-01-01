#include "knh.h"

#include <fstream>
#include <filesystem>

void knh::Entry::read(std::istream& stream)
{
	m_name = kn5::readString(stream);
	m_matrix.read(stream);
	m_children.resize(kn5::readInt32(stream));

	for (auto& child : m_children)
		child.read(stream);
}

void knh::Entry::dump(std::ostream& stream, const std::string& indent) const
{
	stream << indent << "m_name: " << m_name << std::endl;
	m_matrix.dump(stream, indent);
	stream << indent << "children: " << m_children.size() << std::endl;

	for (size_t i = 0; i < m_children.size(); i++)
	{
		stream << indent << "  children[" << i << "]" << std::endl;
		m_children[i].dump(stream, indent + "    ");
	}
}

void knh::read(const std::string& fileName)
{
	std::ifstream stream(fileName, std::ios::binary);

	if (!stream)
		throw std::runtime_error("Couldn't open file: " + fileName);

	m_entry.read(stream);
}

void knh::dump(std::ostream& stream) const
{
	m_entry.dump(stream);
}

bool knh::dump(const std::string& fileName) const
{
	std::ofstream	of(fileName);

	if (of)
	{
		m_entry.dump(of);

		of.close();

		return true;
	}

	return false;
}
