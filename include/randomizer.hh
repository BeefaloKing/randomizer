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
		skills,
		skilldesc,
		TOTAL_FILES
	};
}
static const std::string FILE_NAME[File::TOTAL_FILES] = {"MonPreset.txt", "Levels.txt",
	"skills.txt", "skilldesc.txt"};

namespace Req
{
	enum Req
	{
		none,
		before,
		after
	};
}

class Randomizer
{
public:
	Randomizer() = delete;
	Randomizer(std::string vanillaPath, std::string moddedPath);
	bool writeFiles();

	bool scramble(uint32_t seed);

private:
	void npc();
	void npcHelper(std::vector<std::string> &current, std::vector<std::string> &any,
		size_t* row, size_t rowSize, std::string* name, std::string* nameEnd);

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
	void shuffleHelper(Table &file, size_t col, std::vector<size_t> &rows,
		std::vector<std::string> &values);

	Config &cfg;
	std::string vanillaPath;
	std::string moddedPath;
	Table files[File::TOTAL_FILES]; // Didn't end up needing to keep two coppies around
	Table newFiles[File::TOTAL_FILES]; // Should refactor this out if it causes slowdown
	bool isGood;
};
