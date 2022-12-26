#include "acd.h"

#include <iostream>
#include <fstream>

namespace
{
	int32_t readInt32(std::istream& stream)
	{
		int32_t value;
		stream.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
		return value;
	}

	std::string readString(std::istream& stream, size_t length)
	{
		char* text = new char[length + 1];
		text[length] = 0;
		stream.read(text, length);
		std::string string(text, length);
		delete[] text;
		return string;
	}

	std::string readString(std::istream& stream)
	{
		return readString(stream, readInt32(stream));
	}
}

void acd::read(const std::string& fileName)
{
	calculateKey(std::filesystem::path(fileName).parent_path().filename().string());

	std::ifstream stream(fileName, std::ios::binary);

	if (!stream)
		throw std::runtime_error("Couldn't open file: " + fileName);

	while (stream.peek() != EOF)
	{
		Entry   entry;

		entry.m_name = readString(stream);

		entry.m_rawData.resize(readInt32(stream));
		stream.read(reinterpret_cast<char*>(entry.m_rawData.data()), entry.m_rawData.size() * 4);

		entry.m_data.reserve(entry.m_rawData.size());
		for (size_t i = 0; i < entry.m_rawData.size(); i++)
		{
			entry.m_data.push_back(static_cast<char>(entry.m_rawData[i]) - m_key[i % m_key.size()]);
		}

		m_entries.emplace_back(entry);
	}
}

void acd::dump() const
{
}

void acd::writeEntries(const std::string& directory) const
{
	std::filesystem::path	dataDirectory(directory);

	dataDirectory.append("data");

	if (!std::filesystem::exists(dataDirectory))
		std::filesystem::create_directory(dataDirectory);

	for (const auto& entry : m_entries)
	{
		std::filesystem::path	file = dataDirectory;

		file.append(entry.m_name);

		std::ofstream	fout(file.string());

		if (fout)
			fout.write(entry.m_data.data(), entry.m_data.size());

		fout.close();
	}
}

// from https://github.com/MrElectrify/AssettoCorsaTools/blob/master/AssettoCorsaToolFramework/src/Framework/Files/FileManager.cpp

void acd::calculateKey(const std::string& directory)
{
	/*
	 *	This mimics the behavior of Assetto Corsa's ksSecurity::keyFromString
	 */

	uint8_t factor0 = 0;

	// add all chars, as in AC
	for (auto c : directory)
		factor0 += c;

	// another strange sum
	int32_t factor1 = 0;
	for (size_t i = 0; i < directory.size() - 1; i += 2)
	{
		auto tmp = directory[i] * factor1;
		factor1 = tmp - directory[i + 1];
	}

	// another strange sum
	int32_t factor2 = 0;
	for (size_t i = 1; i < directory.size() - 3; i += 3)
	{
		auto tmp0 = directory[i] * factor2;
		auto tmp1 = tmp0 / (directory[i + 1] + 27);
		factor2 = -27 - directory[i - 1] + tmp1;
	}

	// yet another strange sum
	int8_t factor3 = -125;
	for (size_t i = 1; i < directory.size(); ++i)
	{
		factor3 -= directory[i];
	}

	// of course, another strange sum
	int32_t factor4 = 66;
	for (size_t i = 1; i < directory.size() - 4; i += 4)
	{
		auto tmp = (directory[i] + 15) * factor4;
		factor4 = (directory[i - 1] + 15) * tmp + 22;
	}

	// yup, you guessed it
	uint8_t factor5 = 101;
	for (size_t i = 0; i < directory.size() - 2; i += 2)
	{
		factor5 -= directory[i];
	}

	// not even a purpose in commenting these anymore
	int32_t factor6 = 171;
	for (size_t i = 0; i < directory.size() - 2; i += 2)
	{
		factor6 %= directory[i];
	}

	// last one, finally
	int32_t factor7 = 171;
	for (size_t i = 0; i < directory.size() - 1;)
	{
		auto tmp = factor7 / directory[i];
		factor7 = directory[++i] + tmp;
	}

	constexpr size_t SIZE = 0x100;

	// output in some strange format as in AC
	char tmp[SIZE];
	snprintf(tmp, SIZE, "%d-%d-%d-%d-%d-%d-%d-%d",
		uint8_t(factor0), uint8_t(factor1), uint8_t(factor2), uint8_t(factor3),
		uint8_t(factor4), uint8_t(factor5), uint8_t(factor6), uint8_t(factor7));

	m_key = tmp;
}
