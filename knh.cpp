#include "knh.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>

void knh::Entry::read(std::istream& stream, int depth)
{
	if (depth > kn5::MaxNodeDepth)
		throw std::runtime_error("knh hierarchy nested deeper than " + std::to_string(kn5::MaxNodeDepth) + " levels");

	m_name = kn5::readString(stream);
	m_matrix.read(stream);
	m_children.resize(kn5::readCount(stream, 72));	// name + matrix + child count

	for (auto& child : m_children)
		child.read(stream, depth + 1);
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

	// a short read throws instead of leaving garbage in the values
	stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		m_entry.read(stream);
	}
	catch (const std::ios_base::failure&)
	{
		throw std::runtime_error("Truncated or corrupt file: " + fileName);
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error(fileName + ": " + e.what());
	}
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
