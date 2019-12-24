#pragma once

#include "config.hh"
#include "stub.hh"
#include "table.hh"
#include "utils.hh"
#include <string>

namespace File
{
	enum File
	{
		MonPreset,
		Levels,
		MonStats2,
		UniqueItems,
		MagicPrefix,
		MagicSuffix,
		SetItems,
		Sets,
		ItemTypes,
		Weapons,
		Armor,
		Gems,
		Misc,
		TOTAL_FILES
	};
}
static const std::string FILE_NAME[File::TOTAL_FILES] = {"MonPreset.txt", "Levels.txt",
	"monstats2.txt", "UniqueItems.txt", "MagicPrefix.txt", "MagicSuffix.txt", "SetItems.txt",
	"sets.txt", "ItemTypes.txt", "weapons.txt", "armor.txt", "gems.txt", "misc.txt"};

class Randomizer
{
public:
	Randomizer() = delete;
	Randomizer(std::string vanillaPath, std::string moddedPath);
	bool writeFiles();

	bool scramble(uint32_t seed);

private:
	void npc();
	void npcHelper(Table &pFile, size_t col, std::vector<size_t> &rList,
		std::vector<size_t> &list, std::vector<size_t> &unused, std::vector<std::string> &values);

	void music();
	void mapNames();
	void waypoints();

	void mapLinks();
	// locks are updated when stubs are moved from uList to vList
	void moveNode(std::vector<Stub> &to, std::vector<Stub> &from, uint32_t node, uint32_t locks);
	// placeAfter == true requires source edge to have at least all locks in locks
	// placeAfter == false requires source edge to have at most none of all locks in locks
	void createTree(std::vector<Stub> &vList, std::vector<Stub> &uList, uint32_t locks,
		bool placeAfter);
	void createLoops(std::vector<Stub> &vList, size_t count);
	void linkStubs(Stub &start, Stub &dest);

	void supers();
	void critters();
	void uniques();
	void sets();

	void items();
	void armorHelper(Table &file, std::vector<size_t> rows);
	void weaponHelper(Table &file, std::vector<size_t> rows);

	void gems();
	void shops();
	void shopHelper(Table &file, std::vector<size_t> rows);

	// Shuffle removes values it uses from both rows and values
	// If you'd like randomization with replacement, use fillStrings instead
	void shuffle(Table &file, size_t col, std::vector<size_t> &rows,
		std::vector<std::string> &values);
	// Overload expects multiple cols and tab delimited entries in values
	void shuffle(Table &file, std::vector<size_t> &cols, std::vector<size_t> &rows,
		std::vector<std::string> &values);
	void fillStrings(Table &file, size_t col, const std::vector<std::string> &values);
	void fillRange(Table &file, size_t col, size_t min, size_t max);

	Config &cfg;
	std::string vanillaPath;
	std::string moddedPath;
	Table newFiles[File::TOTAL_FILES];
	bool isGood;
};
