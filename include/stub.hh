#pragma once

#include <cstdint>

// Locks are used to signify which (if any) quests must be completed before traversing a map link
// Locks propogate along adjecent edges further down the tree to mark all edges in a given subtree
namespace Lock
{
	enum Lock : uint32_t
	{
		none = 0x0,
		sunAltar = 0x1,
		compOrb = 0x1<<1,
		anya = 0x1<<2,
		ancients = 0x1<<3,
	};
}

struct Stub
{
	uint32_t levelId;
	uint32_t node;
	uint8_t visList[8];
	uint32_t locks; // Used to keep track of areas locked behind "gatekeeper" quests
};
