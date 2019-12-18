#pragma once

#include <string>

enum EC
{
	missingKey,
	duplicateKey,
	badValue,
	badInstall,
	badAccess,
	badFormat,
	badMpqAccess,
	badMpqFormat,
	infiniteLoop,
	defaultFalse,
	unknown
};

void printError(EC e, const std::string &details);
bool mkdir_r(std::string path);
uint32_t genSeed();
