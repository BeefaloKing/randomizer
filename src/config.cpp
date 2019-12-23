#include "config.hh"
#include "utils.hh"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

Config::Config()
{
	isLoaded = false;
}

bool Config::load(const std::string &file)
{
	if (isLoaded)
	{
		return true;
	}

	std::fstream cFile;
	cFile.open(file, std::fstream::in);
	if (!cFile.good())
	{
		cFile.open(file, std::fstream::out | std::fstream::trunc);
		if (!cFile.good())
		{
			printError(EC::badAccess, file);
			return false;
		}
		exportDefault(cFile);
		cFile.close();
		return false;
	}

	std::string line;
	while (cFile.good())
	{
		std::getline(cFile, line, '\n');
		if (line.length() == 0 || line.at(0) == '#')
		{
			continue;
		}

		std::stringstream ss(line);
		std::string key;
		std::string value;
		std::getline(ss, key, '=');
		std::getline(ss, value);
		if (key.length() > 0 && value.length() > 0)
		{
			if (cfgMap.count(key) == 0)
			{
				cfgMap.emplace(key, value);
			}
			else
			{
				printError(EC::duplicateKey, key);
				cFile.close();
				return false;
			}
		}
	}

	cFile.close();
	return true;
}

bool Config::getString(const std::string &key, std::string &buf)
{
	auto it = cfgMap.find(key);
	if (it != cfgMap.end())
	{
		buf = it->second;
	}
	else
	{
		printError(EC::missingKey, key);
		return false;
	}

	return true;
}

bool Config::getInt(const std::string &key, uint32_t &buf)
{
	auto it = cfgMap.find(key);
	if (it != cfgMap.end())
	{
		std::stringstream ss(it->second);
		ss >> buf;
		if (ss.fail())
		{
			printError(EC::badValue, it->second);
			return false;
		}
	}
	else
	{
		printError(EC::missingKey, key);
		return false;
	}

	return true;
}

// Unlike getString and getInt, return value does not indicate success or failure
// Non existant and/or invalid values default to false (with warning)
bool Config::getBool(const std::string &key)
{
	auto it = cfgMap.find(key);
	if (it != cfgMap.end())
	{
		if (it->second == "true")
		{
			return true;
		}
		else if (it->second != "false")
		{
			printError(EC::defaultFalse, key);
		}
		return false;
	}
	else
	{
		printError(EC::missingKey, key);
		return false;
	}
}

void Config::exportDefault(std::fstream &file)
{
	std::cout << "Exporting default configuration. Please edit config.txt before running again.\n";

	file << "# Lynn's Diablo II Randomizer\n"
	"# Please do not leave any space before or after the equal sign\n\n"
	"d2path=C:/Program Files (x86)/Diablo II\n\n"
	"d2se=false\n"
	"d2se_core=1.13c\n"
	"d2se_mod=Random\n\n"
	"seed=0\n\n"
	"rand_npc=false\n"
	"rand_critters=false\n"
	"rand_mapnames=false\n"
	"rand_wpmenu=false\n"
	"rand_music=false\n\n"
	"rand_maplinks=false\n"
	"force_longmeph=false\n\n"
	"rand_supers=false\n"
	"super_anyact=false\n"
	"super_actboss=false\n\n"
	"rand_uniques=false\n"
	"rand_sets=false\n"
	"rand_itemprops=false\n"
	"rand_gemrunes=false\n";
}
