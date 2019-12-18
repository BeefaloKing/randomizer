#pragma once

#include "stub.hh"

namespace SList
{
	static const Stub ActOne[] =
		{{2, 0, {3, 4, 5, 6, 255}, Lock::none},
		{3, 0, {3, 4, 5, 6, 255}, Lock::none},
		{4, 0, {3, 4, 5, 6, 255}, Lock::none},
		{17, 0, {0, 255}, Lock::none},
		{17, 0, {1, 255}, Lock::none},

		{5, 1, {3, 4, 5, 6, 255}, Lock::none},
		{6, 1, {2, 255}, Lock::none},
		{6, 1, {3, 4, 5, 6, 255}, Lock::none},
		{7, 1, {3, 4, 5, 6, 255}, Lock::none},
		{28, 1, {1, 255}, Lock::none},

		{32, 2, {0, 255}, Lock::none},
		{33, 2, {1, 255}, Lock::none},

		{9, 3, {0, 255}, Lock::none},
		{9, 3, {4, 255}, Lock::none},

		{11, 4, {0, 255}, Lock::none},
		{11, 4, {4, 255}, Lock::none},

		{12, 5, {0, 255}, Lock::none},
		{12, 5, {4, 255}, Lock::none},

		{20, 6, {0, 255}, Lock::none},
		{20, 6, {1, 255}, Lock::none},

		{21, 7, {0, 255}, Lock::none},
		{21, 7, {1, 255}, Lock::none},

		{22, 8, {0, 255}, Lock::none},
		{22, 8, {1, 255}, Lock::none},

		{23, 9, {0, 255}, Lock::none},
		{23, 9, {1, 255}, Lock::none},

		{24, 10, {0, 255}, Lock::none},
		{24, 10, {1, 255}, Lock::none},

		{29, 11, {0, 255}, Lock::none},
		{29, 11, {1, 255}, Lock::none},

		{30, 12, {0, 255}, Lock::none},
		{30, 12, {1, 255}, Lock::none},

		{31, 13, {0, 255}, Lock::none},
		{31, 13, {1, 255}, Lock::none},

		{34, 14, {0, 255}, Lock::none},
		{34, 14, {1, 255}, Lock::none},

		{35, 15, {0, 255}, Lock::none},
		{35, 15, {1, 255}, Lock::none},

		{36, 16, {0, 255}, Lock::none},
		{36, 16, {1, 255}, Lock::none},

		{10, 17, {0, 255}, Lock::none},
		{10, 17, {1, 255}, Lock::none},
		{10, 17, {4, 255}, Lock::none}};
	static const Stub ActOneEnds[] =
		{{8, 18, {0, 255}, Lock::none},
		{13, 19, {1, 255}, Lock::none},
		{14, 20, {1, 255}, Lock::none},
		{15, 21, {1, 255}, Lock::none},
		{16, 22, {1, 255}, Lock::none},
		{18, 23, {0, 255}, Lock::none},
		{19, 24, {0, 255}, Lock::none},
		{37, 25, {0, 255}, Lock::none},
		{25, 26, {0, 255}, Lock::none}};
	static const Stub ActTwo[] =
		{{40, 0, {2, 255}, Lock::none},
		{40, 0, {3, 255}, Lock::none},
		{40, 0, {4, 255}, Lock::sunAltar},
		{41, 0, {0, 1, 2, 3, 255}, Lock::none},
		{42, 0, {0, 1, 2, 3, 255}, Lock::none},
		{43, 0, {4, 255}, Lock::none},
		{44, 0, {2, 255}, Lock::none},
		{45, 0, {1, 255}, Lock::none},

		{46, 1, {1, 255}, Lock::none},
		{46, 1, {2, 255}, Lock::none},
		{46, 1, {3, 255}, Lock::none},
		{46, 1, {4, 255}, Lock::none},
		{46, 1, {5, 255}, Lock::none},
		{46, 1, {6, 255}, Lock::none},
		{46, 1, {7, 255}, Lock::none},
		{54, 1, {0, 255}, Lock::none},
		{54, 1, {1, 255}, Lock::none},

		{50, 2, {0, 255}, Lock::none},
		{50, 2, {2, 255}, Lock::none},
		{50, 2, {3, 255}, Lock::none},

		{47, 3, {0, 255}, Lock::none},
		{47, 3, {1, 255}, Lock::none},
		{47, 3, {2, 255}, Lock::none},

		{51, 4, {0, 255}, Lock::none},
		{51, 4, {1, 255}, Lock::none},
		{51, 4, {2, 255}, Lock::none},
		{51, 4, {3, 255}, Lock::none},

		{52, 5, {0, 255}, Lock::none},
		{52, 5, {1, 255}, Lock::none},
		{52, 5, {2, 255}, Lock::none},
		{52, 5, {3, 255}, Lock::none},

		{53, 6, {0, 255}, Lock::none},
		{53, 6, {1, 255}, Lock::none},
		{53, 6, {2, 255}, Lock::none},
		{53, 6, {3, 255}, Lock::none},

		{48, 7, {0, 255}, Lock::none},
		{48, 7, {2, 255}, Lock::none},

		{55, 8, {0, 255}, Lock::none},
		{55, 8, {4, 255}, Lock::none},

		{56, 9, {0, 255}, Lock::none},
		{56, 9, {4, 255}, Lock::none},

		{57, 10, {0, 255}, Lock::none},
		{57, 10, {4, 255}, Lock::none},

		{58, 11, {0, 255}, Lock::none},
		{58, 11, {4, 255}, Lock::none},

		{62, 12, {0, 255}, Lock::none},
		{62, 12, {1, 255}, Lock::none},

		{63, 13, {0, 255}, Lock::none},
		{63, 13, {1, 255}, Lock::none}};
	static const Stub ActTwoQuest[] =
		{{61, 14, {0, 255}, Lock::none}};
	static const Stub ActTwoEnds[] =
		{{49, 15, {0, 255}, Lock::none},
		{65, 16, {0, 255}, Lock::none},
		{59, 17, {0, 255}, Lock::none},
		{64, 18, {0, 255}, Lock::none},
		{60, 19, {0, 255}, Lock::none},
		{66, 20, {0, 255}, Lock::none},
		{67, 21, {0, 255}, Lock::none},
		{68, 22, {0, 255}, Lock::none},
		{69, 23, {0, 255}, Lock::none},
		{70, 24, {0, 255}, Lock::none},
		{71, 25, {0, 255}, Lock::none},
		{72, 26, {0, 255}, Lock::none}};
	static const Stub ActThree[] =
		{{76, 0, {0, 255}, Lock::none},
		{76, 0, {1, 255}, Lock::none},
		{78, 0, {0, 255}, Lock::none},
		{78, 0, {1, 255}, Lock::none},
		{80, 0, {0, 255}, Lock::none},
		{80, 0, {1, 255}, Lock::none},
		{80, 0, {2, 255}, Lock::none},
		{80, 0, {3, 255}, Lock::none},
		{81, 0, {0, 255}, Lock::none},
		{81, 0, {1, 255}, Lock::none},
		{81, 0, {2, 255}, Lock::none},
		{81, 0, {3, 255}, Lock::none},
		{82, 0, {2, 255}, Lock::none},
		{82, 0, {3, 255}, Lock::none},
		{83, 0, {0, 255}, Lock::compOrb},

		{92, 1, {0, 255}, Lock::none},
		{92, 1, {1, 255}, Lock::none},
		{92, 1, {2, 255}, Lock::none},
		{92, 1, {3, 255}, Lock::none},
		{92, 1, {4, 255}, Lock::none},

		{86, 2, {0, 255}, Lock::none},
		{86, 2, {1, 255}, Lock::none},

		{87, 3, {0, 255}, Lock::none},
		{87, 3, {1, 255}, Lock::none},

		{88, 4, {0, 255}, Lock::none},
		{88, 4, {1, 255}, Lock::none},

		{89, 5, {0, 255}, Lock::none},
		{89, 5, {1, 255}, Lock::none},

		{101, 6, {0, 1, 255}, Lock::none},
		{101, 6, {2, 3, 255}, Lock::none}};
	static const Stub ActThreeQuest[] =
		{{85, 7, {1, 255}, Lock::none},
		{91, 8, {1, 255}, Lock::none},
		{93, 9, {0, 255}, Lock::none}};
	static const Stub ActThreeLockA[] =
		{{100, 10, {0, 1, 255}, Lock::none},
		{100, 10, {2, 3, 255}, Lock::none}};
	static const Stub ActThreeLockB[] =
		{{102, 11, {2, 3, 255}, Lock::none}};
	static const Stub ActThreeEnds[] =
		{{84, 12, {1, 255}, Lock::none},
		{90, 13, {1, 255}, Lock::none},
		{94, 14, {0, 1, 255}, Lock::none},
		{95, 15, {0, 1, 255}, Lock::none},
		{96, 16, {0, 1, 255}, Lock::none},
		{97, 17, {0, 1, 255}, Lock::none},
		{98, 18, {0, 1, 255}, Lock::none},
		{99, 19, {0, 1, 255}, Lock::none}};
	static const Stub ActFive[] =
		{{112, 0, {2, 255}, Lock::none},
		{121, 0, {1, 255}, Lock::anya},

		{113, 1, {0, 255}, Lock::none},
		{113, 1, {1, 255}, Lock::none},
		{113, 1, {2, 255}, Lock::none},

		{115, 2, {0, 255}, Lock::none},
		{115, 2, {1, 255}, Lock::none},
		{115, 2, {2, 255}, Lock::none},

		{118, 3, {0, 255}, Lock::none},
		{118, 3, {1, 255}, Lock::none},
		{118, 3, {2, 255}, Lock::none},

		{117, 4, {0, 255}, Lock::none},
		{117, 4, {2, 255}, Lock::none},

		{122, 5, {0, 255}, Lock::none},
		{122, 5, {1, 255}, Lock::none},

		{128, 7, {0, 255}, Lock::none},
		{128, 7, {1, 255}, Lock::none},

		{129, 8, {0, 255}, Lock::none},
		{129, 8, {1, 255}, Lock::none},

		{130, 9, {0, 255}, Lock::none},
		{130, 9, {1, 255}, Lock::none},

		{120, 10, {0, 255}, Lock::ancients},
		{120, 10, {1, 255}, Lock::ancients}};
	static const Stub ActFiveQuest[] =
		{{114, 13, {0, 255}, Lock::none}};
	static const Stub ActFiveLockA[] =
		{{123, 6, {0, 255}, Lock::none},
		{123, 6, {1, 255}, Lock::none}};
	static const Stub ActFiveLockB[] =
		{{131, 12, {0, 255}, Lock::none}};
	static const Stub ActFiveEnds[] =
		{{116, 11, {0, 255}, Lock::none},
		{119, 14, {0, 255}, Lock::none},
		{124, 15, {0, 255}, Lock::none}};
}
// There has to be a better way
