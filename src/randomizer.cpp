#include "randomizer.hh"
#include "stub.hh"
#include "stublists.hh"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <tuple>
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

	if (cfg.getBool("rand_uniques"))
	{
		std::cout << "Randomizing unique items.\n";
		uniques();
	}

	if (cfg.getBool("rand_sets"))
	{
		std::cout << "Randomizing set items.\n";
		sets();
	}

	if (cfg.getBool("rand_itemprops"))
	{
		std::cout << "Randomizing item properties.\n";
		items();
	}

	if (cfg.getBool("rand_gemrunes"))
	{
		std::cout << "Randomizing gem and rune properties.\n";
		gems();
	}

	if (cfg.getBool("rand_shopstock"))
	{
		std::cout << "Randomizing vendor inventories.\n";
		shops();
	}

	if (cfg.getBool("rand_charskills"))
	{
		std::cout << "Randomizing character skill trees.\n";
		skills();
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

	std::vector<size_t> dummy = {3, 4, 6, 13, 14, 21, 22, 69, 70, 108, 109, 131, 195, 203, 204,
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
	Table &stats = newFiles[File::MonStats2];
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

void Randomizer::uniques()
{
	Table &items = newFiles[File::UniqueItems];
	size_t enabled;
	size_t lvl;
	size_t code;
	size_t image;
	size_t trans;
	size_t prop;
	if (!items.findHeader("enabled", enabled) || !items.findHeader("lvl", lvl) ||
		!items.findHeader("code", code) || !items.findHeader("chrtransform", trans) ||
		!items.findHeader("prop1", prop) || !items.findHeader("flippyfile", image))
	{
		printError(EC::badMpqFormat, items.name());
		return;
	}

	std::vector<size_t> rows;
	std::vector<size_t> rowCopy; // Maybe terrible, but used since shuffle removes everything
	std::vector<size_t> cols;
	std::vector<std::string> values;
	items.findRows(enabled, "1", rows);

	cols.assign({lvl, lvl + 1});
	items.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(items, cols, rowCopy, values);

	// Guessing chaning the index will break things, so change everything else instead
	cols.assign({code, code + 1, image, image + 1});
	items.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(items, cols, rowCopy, values);

	cols.assign({trans, trans + 1});
	items.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(items, cols, rowCopy, values);

	for (size_t i = 0; i < 12; i++) // 12 prop columns
	{
		size_t propCol = prop + (i * 4);
		cols.assign({propCol, propCol + 1, propCol + 2, propCol + 3});
		items.getColValues(rows, cols, values);
		rowCopy = rows;
		shuffle(items, cols, rowCopy, values);
	}
}

void Randomizer::sets()
{
	Table &sItems = newFiles[File::SetItems];
	Table &types = newFiles[File::ItemTypes];
	Table &weapons = newFiles[File::Weapons];
	Table &armor = newFiles[File::Armor];
	size_t setItem;
	size_t typCode;
	size_t typBody;
	size_t wepType;
	size_t wepCode;
	size_t wepHand;
	size_t wepSpwn;
	size_t armType;
	size_t armCode;
	size_t armSpwn;
	if (!sItems.findHeader("item", setItem))
	{
		printError(EC::badMpqFormat, sItems.name());
		return;
	}
	if (!types.findHeader("Code", typCode) || !types.findHeader("BodyLoc1", typBody))
	{
		printError(EC::badMpqFormat, types.name());
		return;
	}
	if (!weapons.findHeader("type", wepType) || !weapons.findHeader("code", wepCode) ||
		!weapons.findHeader("2handed", wepHand) || !weapons.findHeader("spawnable", wepSpwn))
	{
		printError(EC::badMpqFormat, weapons.name());
		return;
	}
	if (!armor.findHeader("type", armType) || !armor.findHeader("code", armCode) ||
		!armor.findHeader("spawnable", armSpwn))
	{
		printError(EC::badMpqFormat, armor.name());
		return;
	}

	// For each set item, randomize the item code to another item equipable in the same slot
	for (size_t i = 0; i < sItems.rows(); i++)
	{
		std::string itemCode = sItems.at(i, setItem);
		std::vector<size_t> rows;

		std::string itemType;
		std::string bodyLoc;
		std::vector<std::string> itemTypes;
		std::vector<std::string> itemCodes;

		weapons.findRows(wepCode, itemCode, rows);
		if (!rows.empty()) // if itemType is a weapon
		{
			itemType = weapons.at(rows.at(0), wepType);
			bool twoHand = weapons.at(rows.at(0), wepHand) == "1";
			rows.clear();

			types.findRows(typCode, itemType, rows);
			bodyLoc = types.at(rows.at(0), typBody);
			rows.clear();

			types.findRows(typBody, bodyLoc, rows);
			types.getColValues(rows, typCode, itemTypes);
			rows.clear();

			for (std::string &e : itemTypes)
			{
				weapons.findRows(wepType, e, rows);
			}

			size_t rowIndex = 0;
			while (rowIndex < rows.size())
			{
				if ((weapons.at(rows.at(rowIndex), wepHand) == "1") != twoHand ||
					weapons.at(rows.at(rowIndex), wepSpwn) != "1")
				{
					rows.at(rowIndex) = rows.back();
					rows.pop_back();
				}
				else
				{
					rowIndex++;
				}
			}
			// Populate itemCodes after any with different handedness or not spawnable are removed
			weapons.getColValues(rows, wepCode, itemCodes);
			rows.clear();
		}
		else // if itemType is not a weapon
		{
			armor.findRows(armCode, itemCode, rows);
			if (rows.empty())
			{

				continue; // Jump to top of loop as item is not a weapon or armor
			}
			itemType = armor.at(rows.at(0), armType);
			rows.clear();

			types.findRows(typCode, itemType, rows);
			bodyLoc = types.at(rows.at(0), typBody);
			rows.clear();

			types.findRows(typBody, bodyLoc, rows);
			types.getColValues(rows, typCode, itemTypes);
			rows.clear();

			for (std::string &e : itemTypes)
			{
				armor.findRows(armType, e, rows);
			}

			size_t rowIndex = 0;
			while (rowIndex < rows.size())
			{
				if (armor.at(rows.at(rowIndex), armSpwn) != "1")
				{
					rows.at(rowIndex) = rows.back();
					rows.pop_back();
				}
				else
				{
					rowIndex++;
				}
			}
			// Populate itemCodes after any not spawnable are removed
			armor.getColValues(rows, armCode, itemCodes);
			rows.clear();
		}

		if (!itemCodes.empty())
		{
			sItems.at(i, setItem) = itemCodes.at(rand() % itemCodes.size());
		}
	}

	size_t index;
	size_t trans;
	size_t prop;
	size_t eol;
	if (!sItems.findHeader("index", index) || !sItems.findHeader("chrtransform", trans) ||
		!sItems.findHeader("prop1", prop) || !sItems.findHeader("*eol", eol))
	{
		printError(EC::badMpqFormat, sItems.name());
		return;
	}

	std::vector<size_t> rows;
	std::vector<size_t> rowCopy; // shuffle wipes the contents of whatever you pass into it
	std::vector<size_t> cols;
	std::vector<std::string> values;

	sItems.findRows(eol, "0", rows); // Ignores the Expansion line

	sItems.getColValues(rows, index, values);
	rowCopy = rows;
	shuffle(sItems, index, rowCopy, values);

	cols.assign({trans, trans + 1});
	sItems.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(sItems, cols, rowCopy, values);

	for (size_t i = 0; i < 19; i++) // 19 prop columns
	{
		size_t propCol = prop + (i * 4);
		cols.assign({propCol, propCol + 1, propCol + 2, propCol + 3});
		sItems.getColValues(rows, cols, values);
		rowCopy = rows;
		shuffle(sItems, cols, rows, values);
	}
	rows.clear();

	Table &sets = newFiles[File::Sets];
	size_t name;
	if (!sets.findHeader("name", name) || !sets.findHeader("PCode2a", prop) ||
		!sets.findHeader("*eol", eol))
	{
		printError(EC::badMpqFormat, sets.name());
		return;
	}

	sets.findRows(eol, "0", rows); // Ignores the Expansion line, again...

	sets.getColValues(rows, name, values);
	rowCopy = rows;
	shuffle(sets, name, rows, values);

	for (size_t i = 0; i < 16; i++) // 16 prop columns
	{
		size_t propCol = prop + (i * 4);
		cols.assign({propCol, propCol + 1, propCol + 2, propCol + 3});
		sets.getColValues(rows, cols, values);
		rowCopy = rows;
		shuffle(sItems, cols, rows, values);
	}
}

void Randomizer::items()
{
	// Item Affixes
	Table &prefix = newFiles[File::MagicPrefix];
	size_t name;
	size_t spawn;
	size_t trans;
	size_t type;
	size_t exclude;
	if (!prefix.findHeader("Name", name) || !prefix.findHeader("spawnable", spawn) ||
		!prefix.findHeader("transform", trans) || !prefix.findHeader("itype1", type) ||
		!prefix.findHeader("etype1", exclude))
	{
		printError(EC::badMpqFormat, prefix.name());
		return;
	}

	std::vector<size_t> rows;
	std::vector<size_t> rowCopy;
	std::vector<size_t> cols;
	std::vector<std::string> values;

	prefix.findRows(spawn, "1", rows);
	prefix.getColValues(rows, name, values);
	rowCopy = rows;
	shuffle(prefix, name, rows, values);

	cols.assign({trans, trans + 1});
	prefix.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(prefix, cols, rows, values);

	for (size_t i = 0; i < 7; i++)
	{
		prefix.getColValues(rows, type + i, values);
		rowCopy = rows;
		shuffle(prefix, type + i, rowCopy, values);
	}
	rows.clear();
	values.assign({""});
	for (size_t i = 0; i < 5; i++) // Probably more fun to not exclude affixes from any items
	{
		fillStrings(prefix, exclude, values);
	}

	Table &suffix = newFiles[File::MagicSuffix];
	if (!suffix.findHeader("Name", name) || !suffix.findHeader("spawnable", spawn) ||
		!suffix.findHeader("transform", trans) || !suffix.findHeader("itype1", type) ||
		!suffix.findHeader("etype1", exclude))
	{
		printError(EC::badMpqFormat, suffix.name());
		return;
	}

	for (size_t i = 0; i < 3; i++)
	{
		fillStrings(suffix, exclude, values);
	}
	values.clear();

	suffix.findRows(spawn, "1", rows);
	suffix.getColValues(rows, name, values);
	rowCopy = rows;
	shuffle(suffix, name, rows, values);

	cols.assign({trans, trans + 1});
	suffix.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(suffix, cols, rows, values);

	for (size_t i = 0; i < 7; i++)
	{
		suffix.getColValues(rows, type + i, values);
		rowCopy = rows;
		shuffle(suffix, type + i, rowCopy, values);
	}
	rows.clear();

	// Armor Properties
	Table &armor = newFiles[File::Armor];
	size_t col = armor.colAt("spawnable");
	std::vector<size_t> current;

	armor.findMatchRows(armor.colAt("code"), armor.colAt("normcode"), current);
	armor.filterRows(col, "1", current, FLT::Contains);
	armorHelper(armor, current);
	current.clear();

	armor.findMatchRows(armor.colAt("code"), armor.colAt("ubercode"), current);
	armor.filterRows(col, "1", current, FLT::Contains);
	armorHelper(armor, current);
	current.clear();

	armor.findMatchRows(armor.colAt("code"), armor.colAt("ultracode"), current);
	armor.filterRows(col, "1", current, FLT::Contains);
	armorHelper(armor, current);
	current.clear();

	// Weapon Properties
	Table &weapons = newFiles[File::Weapons];
	col = weapons.colAt("spawnable");

	weapons.findMatchRows(weapons.colAt("code"), weapons.colAt("normcode"), current);
	weapons.filterRows(col, "1", current, FLT::Contains);
	weaponHelper(weapons, current);
	current.clear();

	weapons.findMatchRows(weapons.colAt("code"), weapons.colAt("ubercode"), current);
	weapons.filterRows(col, "1", current, FLT::Contains);
	weaponHelper(weapons, current);
	current.clear();

	weapons.findMatchRows(weapons.colAt("code"), weapons.colAt("ultracode"), current);
	weapons.filterRows(col, "1", current, FLT::Contains);
	weaponHelper(weapons, current);
	current.clear();
}

void Randomizer::armorHelper(Table &file, std::vector<size_t> rows)
{
	std::vector<size_t> cols;
	std::vector<size_t> rowCopy;
	std::vector<std::string> values;

	cols.assign({file.colAt("speed"), file.colAt("reqstr"), file.colAt("durability"),
		file.colAt("namestr")});
	for (size_t i = 0; i < cols.size(); i++)
	{
		file.getColValues(rows, cols[i], values);
		rowCopy = rows;
		shuffle(file, cols[i], rowCopy, values);
	}

	cols.assign({file.colAt("minac"), file.colAt("maxac"), file.colAt("cost"),
		file.colAt("gamble cost")});
	file.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(file, cols, rowCopy, values);

	cols.assign({file.colAt("level"), file.colAt("levelreq")});
	file.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(file, cols, rowCopy, values);

	cols.assign({file.colAt("Transform"), file.colAt("InvTrans")});
	file.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(file, cols, rowCopy, values);

	rowCopy = rows;
	size_t col = file.colAt("block");
	file.filterRows(col, "0", rowCopy, FLT::NotContains);
	file.getColValues(rowCopy, col, values);
	shuffle(file, col, rowCopy, values);

	rowCopy = rows;
	col = file.colAt("gemsockets");
	file.filterRows(file.colAt("hasinv"), "1", rowCopy, FLT::Contains);
	file.getColValues(rowCopy, col, values);
	// Since many larger items get randomly assigned 1/2 sockets, this gives them a chance at more
	// 1 in 3 "felt right."
	for (size_t i = 0; i < values.size(); i++)
	{
		std::string &entry = values.at(i);
		if ((entry == "1" || entry == "2") && rand() % 3 == 0)
		{
			entry = entry.at(0) + 1;
		}
	}
	shuffle(file, col, rowCopy, values);

	rowCopy = rows;
	cols.assign({file.colAt("mindam"), file.colAt("maxdam"), file.colAt("StrBonus"),
		file.colAt("DexBonus")});
	file.filterRows(cols[0], "0", rowCopy, FLT::NotContains);
	file.getColValues(rowCopy, cols, values);
	shuffle(file, cols, rows, values);
}

void Randomizer::weaponHelper(Table &file, std::vector<size_t> rows)
{
	std::vector<size_t> cols;
	std::vector<size_t> rowCopy;
	std::vector<std::string> values;

	cols.assign({file.colAt("namestr"), file.colAt("speed"), file.colAt("StrBonus"),
		file.colAt("DexBonus"), file.colAt("reqstr"), file.colAt("reqdex"),
		file.colAt("durability")});
	for (size_t i = 0; i < cols.size(); i++)
	{
		file.getColValues(rows, cols[i], values);
		rowCopy = rows;
		shuffle(file, cols[i], rowCopy, values);
	}

	cols.assign({file.colAt("level"), file.colAt("levelreq")});
	file.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(file, cols, rowCopy, values);

	cols.assign({file.colAt("Transform"), file.colAt("InvTrans")});
	file.getColValues(rows, cols, values);
	rowCopy = rows;
	shuffle(file, cols, rowCopy, values);

	rowCopy = rows;
	cols.assign({file.colAt("mindam"), file.colAt("maxdam")});
	file.filterRows(cols[0], "", rowCopy, FLT::NotContains);
	file.getColValues(rowCopy, cols, values);
	shuffle(file, cols, rowCopy, values);

	rowCopy = rows;
	cols.assign({file.colAt("2handmindam"), file.colAt("2handmaxdam")});
	file.filterRows(cols[0], "", rowCopy, FLT::NotContains);
	file.getColValues(rowCopy, cols, values);
	shuffle(file, cols, rowCopy, values);

	rowCopy = rows;
	cols.assign({file.colAt("minmisdam"), file.colAt("maxmisdam")});
	file.filterRows(cols[0], "", rowCopy, FLT::NotContains);
	file.getColValues(rowCopy, cols, values);
	shuffle(file, cols, rowCopy, values);

	rowCopy = rows;
	size_t col = file.colAt("gemsockets");
	file.filterRows(file.colAt("hasinv"), "1", rowCopy, FLT::Contains);
	file.getColValues(rowCopy, col, values);
	for (size_t i = 0; i < values.size(); i++)
	{
		std::string &entry = values.at(i);
		if ((entry == "1" || entry == "2") && rand() % 3 == 0)
		{
			entry = entry.at(0) + 1;
		}
	}
	shuffle(file, col, rowCopy, values);
}

void Randomizer::gems()
{
	Table &gems = newFiles[File::Gems];
	std::vector<size_t> rows;
	std::vector<size_t> rowCopy;
	std::vector<size_t> cols;
	std::vector<std::string> values;

	cols.assign({gems.colAt("nummods"), gems.colAt("transform")});
	gems.findRows(cols[0], "3", rows); // Add all rows with gems
	gems.getColValues(rows, cols[1], values);
	rowCopy = rows;
	shuffle(gems, cols[1], rowCopy, values);

	gems.findRows(cols[0], "1", rows); // Add all rows with runes
	cols.clear();
	values.clear();

	std::vector<std::string> modsOne;
	std::vector<std::string> modsTwo;
	std::vector<std::string> modsThree;
	std::vector<size_t> colCopy;
	size_t mod = gems.colAt("weaponMod1Code");
	for (size_t i = 0; i < 3; i++) // Weapon, Helm/Body, Shield
	{
		for (size_t j = 0; j < 12; j++) // 4 columns for each of the 3 mods
		{
			cols.push_back(mod + (i * 12) + j);
			colCopy.push_back(mod + (i * 12) + j);
		}

		gems.getColValues(rows, cols, values);
		cols.clear();
	}

	size_t maxValues = values.size() / 3;
	while (!values.empty())
	{
		size_t vIndex = rand() % values.size();
		size_t offset = rand() % 3;

		for (size_t i = 0; i < 3; i++)
		{
			if ((i + offset) % 3 == 0 && modsOne.size() < maxValues)
			{
				modsOne.push_back(values.at(vIndex));
				values.at(vIndex) = values.back();
				values.pop_back();
				break;
			}
			else if ((i + offset) % 3 == 1 && modsTwo.size() < maxValues)
			{
				modsTwo.push_back(values.at(vIndex));
				values.at(vIndex) = values.back();
				values.pop_back();
				break;
			}
			else if ((i + offset) % 3 == 2 && modsThree.size() < maxValues)
			{
				modsThree.push_back(values.at(vIndex));
				values.at(vIndex) = values.back();
				values.pop_back();
				break;
			}
		}
	}

	for (size_t i = 0; i < maxValues; i++)
	{
		values.push_back(modsOne.at(i) + "\t" + modsTwo.at(i) + "\t" + modsThree.at(i));
	}

	rowCopy = rows;
	shuffle(gems, colCopy, rowCopy, values);
}

void Randomizer::shops()
{
	std::vector<size_t> rows;
	size_t col;

	/**Table &misc = newFiles[File::Misc];
	col = misc.colAt("dropsound");
	misc.findRows(col, "item_potion", rows);
	misc.findRows(col, "item_book", rows);
	misc.findRows(col, "item_quiver", rows);
	misc.findRows(col, "item_scroll", rows);
	misc.findRows(col, "item_key", rows);

	col = misc.colAt("spawnable");
	misc.filterRows(col, "1", rows, FLT::Contains);
	col = misc.colAt("name");
	misc.filterRows(col, "elixir", rows, FLT::NotContains);
	misc.filterRows(col, "Scroll", rows, FLT::NotContains);

	shopHelper(misc, rows);
	rows.clear();**/

	Table &armor = newFiles[File::Armor];
	armor.findMatchRows(armor.colAt("code"), armor.colAt("normcode"), rows);
	col = armor.colAt("spawnable");
	armor.filterRows(col, "1", rows, FLT::Contains);

	shopHelper(armor, rows);
	rows.clear();

	Table &weapons = newFiles[File::Weapons];
	weapons.findMatchRows(weapons.colAt("code"), weapons.colAt("normcode"), rows);
	col = weapons.colAt("spawnable");
	weapons.filterRows(col, "1", rows, FLT::Contains);
	col = weapons.colAt("type");
	weapons.filterRows(col, "tpot", rows, FLT::NotContains);

	shopHelper(weapons, rows);
	rows.clear();
}

void Randomizer::shopHelper(Table &file, std::vector<size_t> rows)
{
	std::vector<size_t> rowCopy;
	std::vector<size_t> cols;
	std::vector<std::string> values;
	size_t col = file.colAt("CharsiMin");

	for (size_t i = 0; i < 17; i++) // 17 vendors
	{
		size_t vendor = col + (i * 5);
		for (size_t j = 0; j < 5; j++)
		{
			cols.push_back(vendor + j);
		}
		file.getColValues(rows, cols, values);
		rowCopy = rows;
		shuffle(file, cols, rowCopy, values);
		cols.clear();
	}
}

void Randomizer::skills()
{
	Table &skills = newFiles[File::Skills];
	Table &desc = newFiles[File::SkillDesc];

	std::vector<size_t> skillRows;
	std::vector<size_t> levelRows;
	std::vector<std::string> values;

	std::vector<std::string> classCodes = {"ama", "ass", "bar", "dru", "nec", "pal", "sor"};
	std::vector<std::tuple<std::string, std::string, std::string>> classPosData[7];

	for (size_t i = 0; i < classCodes.size(); i++)
	{
		skillsGetPosData(skills, desc, classCodes.at(i), classPosData[i]);
	}

	size_t s_char = skills.colAt("charclass");
	size_t s_level = skills.colAt("reqlevel");
	size_t s_max = skills.colAt("maxlvl");
	skills.findRows(s_max, "20", skillRows);

	for (size_t i = 0; i <= 30; i += 6)
	{
		levelRows = skillRows;
		skills.filterRows(s_level, std::to_string(i > 0 ? i : 1), levelRows, FLT::Contains);
		skills.getColValues(levelRows, s_char, values);
		shuffle(skills, s_char, levelRows, values);
	}
	skillRows.clear();

	for (size_t i = 0; i < classCodes.size(); i++)
	{
		skillsFixPosData(skills, desc, classCodes.at(i), classPosData[i]);
	}

	for (size_t i = 0; i < classCodes.size(); i++)
	{
		skillsFixSyn(skills, desc, classCodes.at(i));
	}

	for (size_t i = 0; i < classCodes.size(); i++)
	{
		skillsFixIcon(skills, desc, classCodes.at(i));
	}

	std::vector<size_t> allColumns; // Should probably overload getColValues() to grab all columns
	for (size_t i = 0; i < skills.cols(); i++)
	{
		allColumns.push_back(i);
	}

	for (size_t i = 0; i < classCodes.size(); i++)
	{
		skills.findRows(s_char, classCodes.at(i), skillRows);
		for (size_t j = 0; j <= 30; j += 6)
		{
			levelRows = skillRows;
			skills.filterRows(s_level, std::to_string(j > 0 ? j : 1), levelRows, FLT::Contains);
			skills.getColValues(levelRows, allColumns, values);
		}
		skillRows.clear();
	}
	skills.findRows(s_max, "20", skillRows);
	skills.importDelimited(allColumns, skillRows, values);
}

void Randomizer::skillsGetPosData(Table &skills, Table &desc, const std::string &classCode,
	std::vector<std::tuple<std::string, std::string, std::string>> &posData)
{
	size_t s_char = skills.colAt("charclass");
	size_t s_skill = skills.colAt("skill");
	size_t s_req = skills.colAt("reqskill1");
	size_t s_desc = skills.colAt("skilldesc");
	size_t d_desc = desc.colAt("skilldesc");
	size_t d_pos = desc.colAt("SkillPage");
	std::vector<size_t> posCols = {d_pos, d_pos + 1, d_pos + 2};

	std::vector<size_t> classRows;
	std::vector<size_t> descRows;
	std::vector<std::string> values;

	skills.findRows(s_char, classCode, classRows);
	for (size_t i = 0; i < classRows.size(); i++)
	{
		std::tuple<std::string, std::string, std::string> entry;
		std::get<0>(entry) = skills.at(classRows.at(i), s_skill);
		std::get<1>(entry) = skills.at(classRows.at(i), s_req);
		std::get<2>(entry) = skills.at(classRows.at(i), s_req + 1);

		skills.findRows(s_skill, std::get<0>(entry), descRows);
		skills.findRows(s_skill, std::get<1>(entry), descRows);
		skills.findRows(s_skill, std::get<2>(entry), descRows);

		std::get<0>(entry) = skills.at(descRows.at(0), s_desc);
		std::get<1>(entry) = descRows.size() > 1 ? skills.at(descRows.at(1), s_desc) : "";
		std::get<2>(entry) = descRows.size() > 2 ? skills.at(descRows.at(2), s_desc) : "";
		descRows.clear();

		desc.findRows(d_desc, std::get<0>(entry), descRows);
		desc.findRows(d_desc, std::get<1>(entry), descRows);
		desc.findRows(d_desc, std::get<2>(entry), descRows);
		desc.getColValues(descRows, posCols, values);
		descRows.clear();

		std::get<0>(entry) = values.at(0);
		std::get<1>(entry) = values.size() > 1 ? values.at(1) : "";
		std::get<2>(entry) = values.size() > 2 ? values.at(2) : "";
		values.clear();
		posData.push_back(entry);
	}
}

void Randomizer::skillsFixPosData(Table &skills, Table &desc, const std::string &classCode,
	std::vector<std::tuple<std::string, std::string, std::string>> &posData)
{
	size_t s_char = skills.colAt("charclass");
	size_t s_level = skills.colAt("reqlevel");
	size_t s_desc = skills.colAt("skilldesc");
	size_t s_skill = skills.colAt("skill");
	size_t s_req = skills.colAt("reqskill1");
	size_t d_desc = desc.colAt("skilldesc");
	size_t d_pos = desc.colAt("SkillPage");
	std::vector<size_t> posCols = {d_pos, d_pos + 1, d_pos + 2};

	std::vector<size_t> skillRows;
	std::vector<size_t> descRows;
	std::vector<std::string> values;
	std::vector<std::tuple<std::string, std::string, std::string>> rowPosData;
	std::vector<std::string> posCopy;

	for (size_t i = 0; i <= 30; i += 6)
	{
		skills.findRows(s_char, classCode, skillRows);
		skills.filterRows(s_level, std::to_string(i > 0 ? i : 1), skillRows, FLT::Contains);
		skills.getColValues(skillRows, s_desc, values);
		for (size_t j = 0; j < values.size(); j++)
		{
			desc.findRows(d_desc, values.at(j), descRows);
			rowPosData.push_back(*posData.begin());
			posCopy.push_back(std::get<0>(*posData.begin()));
			posData.erase(posData.begin());
		}
		skillRows.clear();
		values.clear();

		for (size_t j = 0; j < descRows.size(); j++)
		{
			size_t swapPos = rand() % descRows.size();
			std::swap(rowPosData.at(j), rowPosData.at(swapPos));
			std::swap(posCopy.at(j), posCopy.at(swapPos));
		}
		desc.importDelimited(posCols, descRows, posCopy);
		posCopy.clear();

		for (size_t j = 0; j < descRows.size(); j++)
		{
			skills.findRows(s_desc, desc.at(descRows.at(j), d_desc), skillRows);
			skills.filterRows(s_char, classCode, skillRows, FLT::Contains);
			std::get<0>(rowPosData.at(j)) = skills.at(skillRows.at(0), s_skill);
			skillRows.clear();

			std::vector<size_t> reqRows;
			std::vector<std::string> reqValues;

			std::stringstream position;
			position.str(std::get<1>(rowPosData.at(j)));
			if (position.str() != "")
			{
				std::string entry;
				std::getline(position, entry, '\t');
				desc.findRows(d_pos, entry, reqRows);

				std::getline(position, entry, '\t');
				desc.filterRows(d_pos + 1, entry, reqRows, FLT::Contains);

				std::getline(position, entry, '\t');
				desc.filterRows(d_pos + 2, entry, reqRows, FLT::Contains);

				desc.getColValues(reqRows, d_desc, reqValues);
				for (size_t k = 0; k < reqValues.size(); k++)
				{
					skills.findRows(s_desc, reqValues.at(k), skillRows);
				}
				skills.filterRows(s_char, classCode, skillRows, FLT::Contains);
				std::get<1>(rowPosData.at(j)) = skills.at(skillRows.at(0), s_skill);
				reqRows.clear();
				reqValues.clear();
				skillRows.clear();
				position.clear();
			}

			position.str(std::get<2>(rowPosData.at(j)));
			if (position.str() != "")
			{
				std::string entry;
				std::getline(position, entry, '\t');
				desc.findRows(d_pos, entry, reqRows);

				std::getline(position, entry, '\t');
				desc.filterRows(d_pos + 1, entry, reqRows, FLT::Contains);

				std::getline(position, entry, '\t');
				desc.filterRows(d_pos + 2, entry, reqRows, FLT::Contains);

				desc.getColValues(reqRows, d_desc, reqValues);
				for (size_t k = 0; k < reqValues.size(); k++)
				{
					skills.findRows(s_desc, reqValues.at(k), skillRows);
				}
				skills.filterRows(s_char, classCode, skillRows, FLT::Contains);
				std::get<2>(rowPosData.at(j)) = skills.at(skillRows.at(0), s_skill);
				reqRows.clear();
				reqValues.clear();
				skillRows.clear();
				position.clear();
			}

			skills.findRows(s_skill, std::get<0>(rowPosData.at(j)), skillRows);
			skills.at(skillRows.at(0), s_req) = std::get<1>(rowPosData.at(j));
			skills.at(skillRows.at(0), s_req + 1) = std::get<2>(rowPosData.at(j));
			skillRows.clear();
		}
		descRows.clear();
		rowPosData.clear();
	}
}

void Randomizer::skillsFixSyn(Table &skills, Table &desc, const std::string &classCode)
{
	size_t s_class = skills.colAt("charclass");
	size_t s_desc = skills.colAt("skilldesc");
	size_t s_skill = skills.colAt("skill");
	size_t d_desc = desc.colAt("skilldesc");
	size_t d_text = desc.colAt("dsc3texta2");
	size_t d_name = desc.colAt("str name");

	std::vector<size_t> rows;

	std::vector<std::string> calcHeaders = {"prgcalc1", "prgcalc2", "prgcalc3", "auralencalc",
	"aurarangecalc", "aurastatcalc1", "aurastatcalc2", "aurastatcalc3", "aurastatcalc4",
	"aurastatcalc5", "aurastatcalc6", "passivecalc1", "passivecalc2", "passivecalc3",
	"passivecalc4", "passivecalc5", "sumskill1", "sumsk1calc", "sumskill2", "sumsk2calc",
	"sumskill3", "sumsk3calc", "sumskill4", "sumsk4calc", "sumskill5", "sumsk5calc", "calc1",
	"calc2", "calc3", "calc4", "ToHitCalc", "DmgSymPerCalc", "EDmgSymPerCalc", "ELenSymPerCalc"};
	std::vector<std::string> dCalcHeaders = {"desccalca1", "desccalcb1", "desccalca2",
	"desccalcb2", "desccalca3", "desccalcb3", "desccalca4", "desccalcb4", "desccalca5",
	"desccalcb5", "desccalca6", "desccalcb6", "dsc2calca1", "dsc2calcb1", "dsc2calca2",
	"dsc2calcb2", "dsc2calca3", "dsc2calcb3", "dsc2calca4", "dsc2calcb4", "dsc3calca1",
	"dsc3calcb1", "dsc3calca2", "dsc3calcb2", "dsc3calca3", "dsc3calcb3", "dsc3calca4",
	"dsc3calcb4", "dsc3calca5", "dsc3calcb5", "dsc3calca6", "dsc3calcb6", "dsc3calca7",
	"dsc3calcb7"};
	std::vector<size_t> calcCols;
	std::vector<size_t> dCalcCols;
	for (size_t i = 0; i < calcHeaders.size(); i++)
	{
		calcCols.push_back(skills.colAt(calcHeaders.at(i)));
	}
	for (size_t i = 0; i < dCalcHeaders.size(); i++)
	{
		dCalcCols.push_back(desc.colAt(dCalcHeaders.at(i)));
	}

	skills.findRows(s_class, classCode, rows);

	Table skillsCopy = skills;
	Table descCopy = desc;
	for (size_t i = 0; i < rows.size(); i++)
	{
		size_t row = rows.at(i);
		std::vector<size_t> descRows;
		std::vector<size_t> skillRows;

		desc.findRows(d_desc, skills.at(row, s_desc), descRows);

		for (size_t j = 0; j < 6; j++)
		{
			size_t textCol = d_text + (j * 5);
			std::string synName = desc.at(descRows.at(0), textCol);
			if (synName == "")
			{
				break; // No more synergies (or none to begin with)
			}
			else if (synName.back() == '*') // Synergy is a duplicate
			{
				desc.at(descRows.at(0), textCol).pop_back(); // Remove '*'
				continue; // Do not process
			}

			desc.findRows(d_name, synName, skillRows);
			std::string synDesc = desc.at(skillRows.at(0), d_desc);
			skillRows.clear();

			skills.findRows(s_desc, synDesc, skillRows);
			std::string syn = skills.at(skillRows.at(0), s_skill);
			skillRows.clear();

			size_t randSkill = rows.at(rand() % rows.size());
			std::string skill = skills.at(randSkill, s_skill);
			std::string skillDesc = skills.at(randSkill, s_desc);
			desc.findRows(d_desc, skillDesc, skillRows);
			std::string skillName = desc.at(skillRows.at(0), d_name);
			skillRows.clear();

			for (size_t k = 0; k < calcCols.size(); k++)
			{
				const std::string &calcCopy = skillsCopy.at(row, calcCols.at(k));
				std::string &calcValue = skills.at(row, calcCols.at(k));

				skillsSynHelper(calcCopy, calcValue, syn, skill);
			}

			for (size_t k = 0; k < dCalcCols.size(); k++)
			{
				const std::string &calcCopy = descCopy.at(descRows.at(0), dCalcCols.at(k));
				std::string &calcValue = desc.at(descRows.at(0), dCalcCols.at(k));

				skillsSynHelper(calcCopy, calcValue, syn, skill);
			}

			// Check for duplicate synergies (e.g. Frozen Armor)
			for (size_t k = j + 1; k < 6; k++)
			{
				size_t dupeCol = d_text + (k * 5);
				if (desc.at(descRows.at(0), textCol) == desc.at(descRows.at(0), dupeCol))
				{
					// Mark duplicate synergies with '*'
					desc.at(descRows.at(0), dupeCol) = skillName + "*";
				}
			}
			desc.at(descRows.at(0), textCol) = skillName;
		}
	}
}

void Randomizer::skillsSynHelper(const std::string &calcCopy, std::string &calcValue,
	const std::string &syn, const std::string &skill)
{
	std::string safeSyn = '\'' + syn + '\'';
	size_t pos = calcCopy.find(safeSyn);
	if (pos != std::string::npos)
	{
		std::string safeSkill = '\'' + skill + '\'';
		pos = calcValue.find(safeSyn);
		calcValue.replace(pos, safeSyn.length(), safeSkill);
	}
	else if (calcCopy == syn)
	{
		calcValue = skill;
	}
}

void Randomizer::skillsFixIcon(Table &skills, Table &desc, const std::string &classCode)
{
	size_t s_class = skills.colAt("charclass");
	size_t s_desc = skills.colAt("skilldesc");
	size_t d_desc = desc.colAt("skilldesc");
	size_t d_icon = desc.colAt("IconCel");

	std::vector<size_t> skillRows;
	std::vector<size_t> descRows;
	std::vector<std::string> values;

	skills.findRows(s_class, classCode, skillRows);
	skills.getColValues(skillRows, s_desc, values);
	for (size_t i = 0; i < values.size(); i++)
	{
		desc.findRows(d_desc, values.at(i), descRows);
	}
	values.clear();

	for (size_t i = 0; i < 30; i++)
	{
		values.push_back(std::to_string(i * 2));
	}
	shuffle(desc, d_icon, descRows, values);
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

void Randomizer::shuffle(Table &file, std::vector<size_t> &cols, std::vector<size_t> &rows,
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
