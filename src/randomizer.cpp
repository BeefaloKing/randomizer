#include "randomizer.hh"
#include "stub.hh"
#include "stublists.hh"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>

Randomizer::Randomizer(std::string vanillaPath, std::string moddedPath) :
	cfg(Config::getInstance())
{
	this->vanillaPath = vanillaPath;
	this->moddedPath = moddedPath;
	isGood = true;
	for (size_t i = 0; i < File::TOTAL_FILES; i++)
	{
		if (!newFiles[i].open(vanillaPath + "/" + FILE_NAME[i]))
		{
			isGood = false;
		}
	}
}

bool Randomizer::writeFiles()
{
	std::cout << "\nExporting modded data files.\n";

	std::fstream out;
	for (size_t i = 0; i < File::TOTAL_FILES; i++)
	{
		out.open(moddedPath + "/" + FILE_NAME[i], std::fstream::out | std::fstream::trunc |
			std::fstream::binary);
		if (out.good())
		{
			newFiles[i].print(out);
		}
		if (!out.good())
		{
			printError(EC::badAccess, moddedPath + "/" + FILE_NAME[i]);
			return false;
		}
		out.close();
	}

	return true;
}

bool Randomizer::scramble(uint32_t seed)
{
	if (!isGood)
	{
		return false;
	}

	std::cout << "\nRandom seed: " << seed << "\n";
	srand(seed);

	if (cfg.getBool("rand_npc"))
	{
		std::cout << "Randomizing NPC spawn locations.\n";
		npc();
	}

	if (cfg.getBool("rand_music"))
	{
		std::cout << "Randomizing music.\n";
		music();
	}

	if (cfg.getBool("rand_mapnames"))
	{
		std::cout << "Randomizing area names.\n";
		mapNames();
	}

	if (cfg.getBool("rand_wpmenu"))
	{
		std::cout << "Randomizing order of waypoint menu.\n";
		waypoints();
	}

	if (cfg.getBool("rand_maplinks"))
	{
		std::cout << "Randomizing area entrances.\n";
		mapLinks();
	}

	if (cfg.getBool("rand_supers"))
	{
		std::cout << "Randomizing super unique spawn locations.\n";
		supers();
	}

	if (cfg.getBool("rand_critters"))
	{
		std::cout << "Randomizing critter spawns.\n";
		critters();
	}

	return true;
}

// Should grab names from file rather than hardcoding strings here
void Randomizer::npc()
{
	Table& pFile = newFiles[File::MonPreset];
	size_t placeCol;
	if (!pFile.findHeader("Place", placeCol))
	{
		printError(EC::badMpqFormat, pFile.name());
		return;
	}

	std::vector<size_t> dummy = {3, 4, 6, 13, 14, 21, 22, 69, 70, 73, 108, 109, 131, 195, 203, 204,
		205, 221};
	std::vector<size_t> actOneR = {2, 5, 7, 8}; // "R" denotes "Restricted"
	std::vector<size_t> actOne = {0};
	std::vector<size_t> actTwoR = {47, 48, 49, 58, 67, 72, 73};
	std::vector<size_t> actTwo = {50, 55, 56, 57, 59};
	std::vector<size_t> actThreeR = {106, 110, 112, 113, 114};
	std::vector<size_t> actThree = {121};
	std::vector<size_t> actFourR = {152, 170};
	std::vector<size_t> actFour = {155, 156};
	std::vector<size_t> actFiveR = {175, 177, 194};
	std::vector<size_t> actFive = {176};

	std::vector<size_t> anyAct;
	std::vector<std::string> values;

	pFile.getColValues(dummy, placeCol, values);
	shuffle(pFile, placeCol, dummy, values);

	npcHelper(pFile, placeCol, actOneR, actOne, anyAct, values);
	npcHelper(pFile, placeCol, actTwoR, actTwo, anyAct, values);
	npcHelper(pFile, placeCol, actThreeR, actThree, anyAct, values);
	npcHelper(pFile, placeCol, actFourR, actFour, anyAct, values);
	npcHelper(pFile, placeCol, actFiveR, actFive, anyAct, values);

	// Shuffle the unrestricted npcs into any remaining locations
	shuffle(pFile, placeCol, anyAct, values);
}

void Randomizer::npcHelper(Table &pFile, size_t col, std::vector<size_t> &rList,
	std::vector<size_t> &list, std::vector<size_t> &unused, std::vector<std::string> &values)
{
	std::vector<std::string> rValues;

	pFile.getColValues(list, col, values);
	pFile.getColValues(rList, col, rValues);
	list.insert(list.end(), rList.begin(), rList.end());
	shuffle(pFile, col, list, rValues);
	unused.insert(unused.end(), list.begin(), list.end());
}

void Randomizer::music()
{
	Table &levels = newFiles[File::Levels];
	size_t musicCol;
	if (!levels.findHeader("SoundEnv", musicCol))
	{
		printError(EC::badMpqFormat, FILE_NAME[File::Levels]);
		return;
	}

	for (size_t i = 0; i < levels.rows(); i++)
	{
		std::string &entry = levels.at(i, musicCol);
		if (entry != "" && entry != "0")
		{
			entry = std::to_string(rand() % 49 + 1);
		}
	}
}

void Randomizer::mapNames()
{
	Table &levels = newFiles[File::Levels];
	size_t actCol;
	size_t nameCol;
	size_t warpCol;
	size_t entryCol;
	if (!levels.findHeader("Act", actCol) || !levels.findHeader("LevelName", nameCol) ||
		!levels.findHeader("LevelWarp", warpCol) || !levels.findHeader("EntryFile", entryCol))
	{
		printError(EC::badMpqFormat, FILE_NAME[File::Levels]);
		return;
	}
	std::vector<size_t> cols = {nameCol, warpCol, entryCol};

	std::vector<size_t> rows;
	std::vector<std::string> values;

	for (size_t act = 0; act < 5; act++)
	{
		levels.findRows(actCol, std::to_string(act), rows);
		if (act == 0)
		{
			size_t i = 0;
			while (i < rows.size())
			{
				if (levels.at(rows.at(i), nameCol) == "" ||
					levels.at(rows.at(i), nameCol) == "Den of Evil")
				{
					rows.at(i) = rows.back();
					rows.pop_back();
				}
				else
				{
					i++;
				}
			}
		}
		levels.getColValues(rows, cols, values);
		shuffle(levels, cols, rows, values);
	}
}

void Randomizer::waypoints()
{
	Table &newLevels = newFiles[File::Levels];
	size_t actCol;
	size_t wpCol;
	if (!newLevels.findHeader("Waypoint", wpCol) || !newLevels.findHeader("Act", actCol))
	{
		printError(EC::badMpqFormat, FILE_NAME[File::Levels]);
		return;
	}

	std::vector<size_t> rows;
	std::vector<std::string> wps;

	for (size_t act = 0; act < 5; act++)
	{
		for (size_t row = 0; row < newLevels.rows(); row++)
		{
			std::string &wpEntry = newLevels.at(row, wpCol);
			// Hardcoded checks to exclude town wp's from being randomized
			if (newLevels.at(row, actCol) == std::to_string(act) && wpEntry != "255" &&
				wpEntry != "" && wpEntry != "0" && wpEntry != "9" && wpEntry != "18" &&
				wpEntry != "27" && wpEntry != "30")
			{
				rows.push_back(row);
				wps.push_back(wpEntry);
			}
		}
		shuffle(newLevels, wpCol, rows, wps);
	}
}

// Good luck with this one
void Randomizer::mapLinks()
{
	Table &newLevels = newFiles[File::Levels];
	size_t idCol;
	size_t visCol;
	if (!newLevels.findHeader("Id", idCol) || !newLevels.findHeader("Vis0", visCol))
	{
		printError(EC::badMpqFormat, FILE_NAME[File::Levels]);
		return;
	}

	std::vector<Stub> uList;
	std::vector<Stub> vList;
	std::vector<Stub> tempList;

	// Act 1
	uList.assign(SList::ActOne, std::end(SList::ActOne));
	moveNode(vList, uList, 0, Lock::none);
	createTree(vList, uList, Lock::none, false);

	uList.assign(SList::ActOneEnds, std::end(SList::ActOneEnds));
	createTree(vList, uList, Lock::none, false);

	// Act 2
	uList.assign(SList::ActTwo, std::end(SList::ActTwo));
	moveNode(vList, uList, 0, Lock::none);
	createTree(vList, uList, Lock::none, false);

	uList.assign(SList::ActTwoQuest, std::end(SList::ActTwoQuest));
	createTree(vList, uList, Lock::sunAltar, false);

	uList.assign(SList::ActTwoEnds, std::end(SList::ActTwoEnds));
	size_t count = (vList.size() - uList.size()) / 2;
	createLoops(vList, count);
	createTree(vList, uList, Lock::none, true);

	// Act 3
	uList.assign(SList::ActThree, std::end(SList::ActThree));
	if (cfg.getBool("force_longmeph"))
	{
		uint32_t extraNode = uList.at(rand() % 15 + 15).node;
		moveNode(tempList, uList, extraNode, Lock::none);
	}
	moveNode(vList, uList, 0, Lock::none);
	createTree(vList, uList, Lock::none, false);

	uList.assign(SList::ActThreeQuest, std::end(SList::ActThreeQuest));
	createTree(vList, uList, Lock::compOrb, false);

	uList.assign(SList::ActThreeLockA, std::end(SList::ActThreeLockA));
	if (tempList.size() > 0) // If force_longmeph, place extra area here
	{
		moveNode(uList, tempList, tempList.at(0).node, Lock::none);
	}
	createTree(vList, uList, Lock::compOrb, true);
	uList.assign(SList::ActThreeLockB, std::end(SList::ActThreeLockB));
	createTree(vList, uList, Lock::compOrb, true);

	uList.assign(SList::ActThreeEnds, std::end(SList::ActThreeEnds));
	count = (vList.size() - uList.size()) / 2;
	createLoops(vList, count);
	createTree(vList, uList, Lock::none, false);

	// Act 5
	uList.assign(SList::ActFive, std::end(SList::ActFive));
	// Ensure valid config exists where both Baal and Anya can be reached
	uint32_t triNode = uList.at(rand() % 9 + 2).node; // Select random area with 3 exits
	uint32_t dualNode = uList.at(rand() % 12 + 11).node; // Select random area with 2 exits
	moveNode(tempList, uList, triNode, Lock::none);
	moveNode(tempList, uList, dualNode, Lock::none);

	moveNode(vList, uList, 0, Lock::none);
	createTree(vList, tempList, Lock::anya, false); // Stubs not locked by Anya now always > 1
	createTree(vList, uList, Lock::none, false);

	uList.assign(SList::ActFiveLockA, std::end(SList::ActFiveLockA));
	createTree(vList, uList, Lock::anya, true);

	uList.assign(SList::ActFiveLockB, std::end(SList::ActFiveLockB));
	createTree(vList, uList, Lock::ancients, true);

	uList.assign(SList::ActFiveQuest, std::end(SList::ActFiveQuest));
	createTree(vList, uList, Lock::anya, false);

	uList.assign(SList::ActFiveEnds, std::end(SList::ActFiveEnds));
	createTree(vList, uList, Lock::none, false);
}

void Randomizer::moveNode(std::vector<Stub> &to, std::vector<Stub> &from, uint32_t node,
	uint32_t locks)
{
	size_t uIndex = 0;
	while (uIndex < from.size())
	{
		if (from.at(uIndex).node == node)
		{
			uint32_t &oldFlags = from.at(uIndex).locks;
			oldFlags = oldFlags | locks;
			to.push_back(from.at(uIndex));
			from.at(uIndex) = from.back();
			from.pop_back();
		}
		else
		{
			uIndex++;
		}
	}
}

void Randomizer::createTree(std::vector<Stub> &vList, std::vector<Stub> &uList, uint32_t locks,
	bool placeAfter)
{
	Table &newLevels = newFiles[File::Levels];
	size_t idCol;
	size_t visCol;
	newLevels.findHeader("Id", idCol);
	newLevels.findHeader("Vis0", visCol);

	size_t retryCount = 0;
	while (!uList.empty() && !vList.empty())
	{
		if (retryCount > 255)
		{
			printError(EC::infiniteLoop, "Randomizer::createTree()");
			return;
		}
		size_t sIndex = rand() % vList.size();
		size_t dIndex = rand() % uList.size();

		Stub &start = vList[sIndex];
		Stub &dest = uList[dIndex];
		// Retry if selected edge does not meet placement requirements
		if ((locks & start.locks) == locks && locks != Lock::none && !placeAfter)
		{
			retryCount++;
			continue;
		}
		else if ((locks & start.locks) != locks && placeAfter)
		{
			retryCount++;
			continue;
		}
		retryCount = 0;

		linkStubs(start, dest);

		// Save dest.node, start.locked, and remove stubs from lists
		uint32_t destNode = dest.node;
		uint32_t locks = start.locks;
		vList.erase(vList.begin() + sIndex);
		uList.erase(uList.begin() + dIndex);

		// Move adjecent stubs to vList
		// If source was locked, lock stubs in the destination node
		moveNode(vList, uList, destNode, locks);
	}
}

void Randomizer::createLoops(std::vector<Stub> &vList, size_t count)
{
	Table &newLevels = newFiles[File::Levels];
	size_t idCol;
	size_t visCol;
	newLevels.findHeader("Id", idCol);
	newLevels.findHeader("Vis0", visCol);

	size_t retryCount = 0;
	while (count > 0)
	{
		if (retryCount > 255)
		{
			printError(EC::infiniteLoop, "Randomizer::createLoops()");
			return;
		}
		size_t sIndex = rand() % vList.size();
		size_t dIndex = rand() % vList.size();

		Stub &start = vList[sIndex];
		Stub &dest = vList[dIndex];
		// Retry if selected edge is a self loop or connects locked/unlocked areas together
		// Prefer to not link between the same node, but if not possible fallback to levelId
		if (retryCount < 32 && (start.node == dest.node || start.locks != dest.locks))
		{
			retryCount++;
			continue;
		}
		else if (retryCount >= 32 && (start.levelId == dest.levelId || start.locks != dest.locks))
		{
			retryCount++;
			continue;
		}
		retryCount = 0;

		linkStubs(start, dest);

		// Remove stubs from lists
		if (sIndex > dIndex)
		{
			vList.erase(vList.begin() + sIndex);
			vList.erase(vList.begin() + dIndex);
		}
		else
		{
			vList.erase(vList.begin() + dIndex);
			vList.erase(vList.begin() + sIndex);
		}

		count--;
	}
}

void Randomizer::linkStubs(Stub &start, Stub &dest)
{
	// This has been copy/pasted a lot
	// Should probably just pass these values to each helper
	Table &newLevels = newFiles[File::Levels];
	size_t idCol;
	size_t visCol;
	newLevels.findHeader("Id", idCol);
	newLevels.findHeader("Vis0", visCol);

	size_t sRow = start.levelId;
	size_t dRow = dest.levelId;
	// Correct for Expansion line causing row/id mismatch
	if (newLevels.at(sRow, idCol) != std::to_string(sRow))
	{
		sRow ++;
	}
	if (newLevels.at(dRow, idCol) != std::to_string(dRow))
	{
		dRow ++;
	}

	// Connect start and dest stubs together
	for (size_t i = 0; start.visList[i] != 255; i++)
	{
		newLevels.at(sRow, visCol + start.visList[i]) = std::to_string(dest.levelId);
	}
	for (size_t i = 0; dest.visList[i] != 255; i++)
	{
		newLevels.at(dRow, visCol + dest.visList[i]) = std::to_string(start.levelId);
	}
}

void Randomizer::supers()
{
	Table &pFile = newFiles[File::MonPreset];
	size_t placeCol;
	if (!pFile.findHeader("Place", placeCol))
	{
		printError(EC::badMpqFormat, pFile.name());
		return;
	}
	bool doBoss = cfg.getBool("super_actboss");
	bool anyAct = cfg.getBool("super_anyact");

	size_t andariel = 9;
	std::vector<size_t> actOne = {12, 34, 35, 36, 37, 38, 39, 40, 41, 43, 44};
	size_t duriel = 66;
	std::vector<size_t> actTwo = {64, 65, 93, 94, 95, 96, 97, 98, 99, 100};
	size_t mephisto = 128;
	std::vector<size_t> actThreeHC = {139, 140, 142};
	std::vector<size_t>	actThreeA = {134, 135, 136};
	std::vector<size_t> actThreeB = {137, 138, 141, 143, 144};
	std::vector<size_t> actFour = {158, 172};
	std::vector<size_t> actFive = {179, 200, 208, 209, 210, 211, 212, 213, 216, 219};
	std::vector<size_t> ancients = {197, 198, 199};

	std::vector<size_t> pool; // Used when anyAct == true to keep track of placements
	std::vector<std::string> values; // Strings read from pFile
	std::vector<std::string> rValues; // "Restricted" values with specific placement restrictions

	pFile.getColValues(actOne, placeCol, values);
	if (doBoss)
	{
		actOne.push_back(andariel);
		rValues.push_back(pFile.at(andariel, placeCol));
		shuffle(pFile, placeCol, actOne, rValues);
	}
	if (!anyAct)
	{
		shuffle(pFile, placeCol, actOne, values);
	}

	pFile.getColValues(actTwo, placeCol, values);
	if (doBoss)
	{
		actTwo.push_back(duriel);
		rValues.push_back(pFile.at(duriel, placeCol));
		shuffle(pFile, placeCol, actTwo, rValues);
	}
	if (!anyAct)
	{
		shuffle(pFile, placeCol, actTwo, values);
	}

	// Guarantee High Council accessible before placing the rest of Act 3
	pFile.getColValues(actThreeA, placeCol, values);
	actThreeA.insert(actThreeA.begin(), actThreeHC.begin(), actThreeHC.end());
	pFile.getColValues(actThreeHC, placeCol, rValues);
	if (!anyAct)
	{
		shuffle(pFile, placeCol, actThreeA, rValues);
	}
	else // if (anyAct) Place High Council anywhere previously visited (including A1 and A2)
	{
		pool.insert(pool.end(), actOne.begin(), actOne.end());
		pool.insert(pool.end(), actTwo.begin(), actTwo.end());
		pool.insert(pool.end(), actThreeA.begin(), actThreeA.end());
		shuffle(pFile, placeCol, pool, rValues);
		// actThreeB may not be accessible before Travincal
		// Added here after High Council have already been placed
		pool.insert(pool.end(), actThreeB.begin(), actThreeB.end());
	}

	actThreeA.insert(actThreeA.end(), actThreeB.begin(), actThreeB.end());
	pFile.getColValues(actThreeB, placeCol, values);
	if (doBoss)
	{
		actThreeA.push_back(mephisto);
		rValues.push_back(pFile.at(mephisto, placeCol));
		shuffle(pFile, placeCol, actThreeA, rValues);
	}
	if (!anyAct)
	{
		shuffle(pFile, placeCol, actThreeA, values);
	}

	// No need to check doBoss since Diablo and Baal cannot be moved regardless
	if (!anyAct)
	{
		pFile.getColValues(actFour, placeCol, values);
		shuffle(pFile, placeCol, actFour, values);

		pFile.getColValues(actFive, placeCol, values);
		shuffle(pFile, placeCol, actFive, values);
	}
	else // if (anyAct)
	{
		// Here we finally place all super uniques with no placement restrictions
		pFile.getColValues(actFour, placeCol, values);
		pFile.getColValues(actFive, placeCol, values);

		shuffle(pFile, placeCol, pool, values);
		shuffle(pFile, placeCol, actFour, values);
		shuffle(pFile, placeCol, actFive, values);
	}

	// Randomize permutation of ancient statues
	pFile.getColValues(ancients, placeCol, values);
	shuffle(pFile, placeCol, ancients, values);

}

void Randomizer::critters()
{
	Table &stats = newFiles[File::monstats2];
	Table &levels = newFiles[File::Levels];
	size_t idCol;
	size_t critterCol;
	size_t cmonCol;
	size_t cpctCol;
	if (!stats.findHeader("Id", idCol) || !stats.findHeader("critter", critterCol))
	{
		printError(EC::badMpqFormat, stats.name());
		return;
	}
	if (!levels.findHeader("cmon1", cmonCol) || !levels.findHeader("cpct1", cpctCol))
	{
		printError(EC::badMpqFormat, levels.name());
		return;
	}

	std::vector<size_t> rows;
	std::vector<std::string> critters;
	stats.findRows(critterCol, "1", rows);
	stats.getColValues(rows, idCol, critters);

	fillStrings(levels, cmonCol, critters);
	fillStrings(levels, cmonCol + 1, critters);
	fillStrings(levels, cmonCol + 2, critters);
	fillStrings(levels, cmonCol + 3, critters);

	fillRange(levels, cpctCol, 1, 100);
	fillRange(levels, cpctCol + 1, 1, 100);
	fillRange(levels, cpctCol + 2, 1, 100);
	fillRange(levels, cpctCol + 3, 1, 100);
}

void Randomizer::shuffle(Table &file, size_t col, std::vector<size_t> &rows,
	std::vector<std::string> &values)
{
	while (!rows.empty() && !values.empty())
	{
		size_t &row = rows.at(rand() % rows.size());
		std::string &value = values.at(rand() % values.size());
		file.at(row, col) = value;

		row = rows.back();
		rows.pop_back();
		value = values.back();
		values.pop_back();
	}
}

void Randomizer::shuffle(Table &file, std::vector<size_t> cols, std::vector<size_t> &rows,
	std::vector<std::string> &values)
{
	while (!rows.empty() && !values.empty())
	{
		size_t &row = rows.at(rand() % rows.size());
		std::string &value = values.at(rand() % values.size());
		std::stringstream entries{value};
		std::string entry;
		for (size_t i = 0; i < cols.size(); i++)
		{
			std::getline(entries, entry, '\t');
			file.at(row, cols.at(i)) = entry;
		}

		row = rows.back();
		rows.pop_back();
		value = values.back();
		values.pop_back();
	}
}

void Randomizer::fillStrings(Table &file, size_t col, const std::vector<std::string> &values)
{
	for (size_t i = 0; i < file.rows(); i++)
	{
		if (file.at(i, 0) != "Expansion")
		{
			file.at(i, col) = values.at(rand() % values.size());
		}
	}
}

void Randomizer::fillRange(Table &file, size_t col, size_t min, size_t max)
{
	for (size_t i = 0; i < file.rows(); i++)
	{
		if (file.at(i, 0) != "Expansion")
		{
			file.at(i, col) = std::to_string(rand() % (max - min + 1) + min);
		}
	}
}
