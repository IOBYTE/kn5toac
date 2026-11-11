#include "acd.h"
#include "kn5.h"

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

void acd::read(const std::string& fileName)
{
	calculateKey(std::filesystem::path(fileName).parent_path().filename().string());

	std::ifstream stream(fileName, std::ios::binary);

	if (!stream)
		throw std::runtime_error("Couldn't open file: " + fileName);

	// a short read throws instead of leaving garbage in the values
	// (peek() at the end only sets eofbit, so the loop below still ends normally)
	stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		if (kn5::readInt32(stream) == -1111)
			kn5::readInt32(stream);
		else
			stream.seekg(0, stream.beg);

		while (stream.peek() != EOF)
		{
			Entry   entry;

			entry.m_name = kn5::readString(stream);

			entry.m_rawData.resize(kn5::readCount(stream, sizeof(int32_t)));
			stream.read(reinterpret_cast<char*>(entry.m_rawData.data()), static_cast<std::streamsize>(entry.m_rawData.size() * sizeof(int32_t)));

			entry.m_data.reserve(entry.m_rawData.size());
			for (size_t i = 0; i < entry.m_rawData.size(); i++)
			{
				char data = static_cast<char>(entry.m_rawData[i]) - m_key[i % m_key.size()];
				if (data != '\r')
					entry.m_data.push_back(data);
			}

			m_entries.emplace_back(entry);
		}
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

void acd::dump(std::ostream& stream) const
{
	stream << "key:     " << m_key << std::endl;
	stream << "entries: " << m_entries.size() << std::endl;
	size_t count = 0;
	for (const auto& entry : m_entries)
	{
		stream << "  entries[" << count++ << "]" << std::endl;
		entry.dump(stream);
	}
}

void acd::Entry::dump(std::ostream& stream) const
{
	stream << "    name:    " << m_name << std::endl;
	stream << "    rawData: " << m_rawData.size() << std::endl;
	stream << "    data:    " << m_data.size() << std::endl;
}

bool acd::dump(const std::string& fileName) const
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

void acd::writeEntries(const std::string& directory) const
{
	std::filesystem::path	dataDirectory(directory);

	for (const auto& entry : m_entries)
	{
		// entry names come from the file: never let one escape the directory
		if (!kn5::isPlainFileName(entry.m_name))
		{
			std::cerr << "Warning: skipping data.acd entry with unsafe name: " << entry.m_name << std::endl;
			continue;
		}

		std::filesystem::path	iniFilePath = dataDirectory;

		iniFilePath.append(entry.m_name);

		std::ofstream	fout(iniFilePath.string());

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
	for (size_t i = 0; i + 1 < directory.size(); i += 2)
	{
		auto tmp = directory[i] * factor1;
		factor1 = tmp - directory[i + 1];
	}

	// another strange sum
	int32_t factor2 = 0;
	for (size_t i = 1; i + 3 < directory.size(); i += 3)
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
	for (size_t i = 1; i + 4 < directory.size(); i += 4)
	{
		auto tmp = (directory[i] + 15) * factor4;
		factor4 = (directory[i - 1] + 15) * tmp + 22;
	}

	// yup, you guessed it
	uint8_t factor5 = 101;
	for (size_t i = 0; i + 2 < directory.size(); i += 2)
	{
		factor5 -= directory[i];
	}

	// not even a purpose in commenting these anymore
	int32_t factor6 = 171;
	for (size_t i = 0; i + 2 < directory.size(); i += 2)
	{
		factor6 %= directory[i];
	}

	// last one, finally
	int32_t factor7 = 171;
	for (size_t i = 0; i + 1 < directory.size();)
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
