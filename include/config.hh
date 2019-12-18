#pragma once

#include <fstream>
#include <map>
#include <string>

class Config
{
public:
	static inline Config &getInstance()
	{
		static Config instance;
		return instance;
	};
	bool load(const std::string &file);

	bool getString(const std::string &key, std::string &buf);
	bool getInt(const std::string &key, uint32_t &buf);
	bool getBool(const std::string &key);
private:
	Config();
	Config(const Config &) = delete;
	void operator=(const Config &) = delete;

	void exportDefault(std::fstream &file);

	std::map<std::string, std::string> cfgMap;
	bool isLoaded;
};
