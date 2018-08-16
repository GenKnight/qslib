#ifndef _QSLIB_CONFIGURATION_H_
#define _QSLIB_CONFIGURATION_H_

#include <map>
#include <string>
#include <fstream>

#include "ValueStorage.h"

namespace qs
{
	class Configuration
	{
	public:
		Configuration() {}

		Configuration(const std::string& filepath)
		{
			std::ifstream is(filepath);
			std::string buffer;
			while (!is.eof())
			{
				std::getline(is, buffer);
				auto pos = buffer.find(' ');
				std::string key = buffer.substr(0, pos);
				std::string value = buffer.substr(pos + 1, buffer.size() - pos);
				m_args.insert(make_pair(std::move(key), std::move(value)));
			}
		}

		std::string& operator[](const std::string& key)
		{
			return m_args.at(key);
		}
	private:
		std::map<std::string, std::string> m_args;
	};
}

#endif