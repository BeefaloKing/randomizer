#include "utils.hh"
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

void printError(EC e, const std::string &details)
{
	switch (e)
	{
		case missingKey:
			std::cout << "Error: Required key \"" << details << "\" missing in config.txt.\n";
			break;
		case duplicateKey:
			std::cout << "Error: Duplicate key \"" << details << "\" found in config.txt.\n";
			break;
		case badValue:
			std::cout << "Error: Improper value \"" << details << "\" assigned to config key.\n";
			break;
		case badInstall:
			std::cout << "Error: Invalid " << details << " installation directory.\n";
			break;
		case badAccess:
			std::cout << "Error: Unable to access \"" << details << "\".\n";
			break;
		case badFormat:
			std::cout << "Error: Invalid file format at \"" << details << "\".\n";
			break;
		case badMpqAccess:
			std::cout << "Error: Unable to access \"" << details << "\" from mpq archive.\n";
			break;
		case badMpqFormat:
			std::cout << "Error: Invalid file format at \"" << details <<
				"\" extracted from mpq archive.\n";
			break;
		case infiniteLoop:
			std::cout << "Error: Infinite loop detected in " << details << ". Please submit a bug"
			" report including your config.txt and randomizer/seed.txt files.\n";
			break;
		case defaultFalse:
			std::cout << "Warning: Non-boolean value assigned to \"" << details << "\".\n";
			break;
		default:
			std::cout << "Error: Unknown error.\n";
			break;
	}
}

// Requires a fully qualified path
bool mkdir_r(std::string path)
{
	std::string subPath = "";
	std::stringstream fullPath(path);
	// An extra getline here to grab the drive letter
	std::getline(fullPath, subPath, '/');

	std::string dir = "";
	struct stat status;
	while (fullPath.good())
	{
		std::getline(fullPath, dir, '/');
		subPath += "/" + dir;

		stat(subPath.c_str(), &status);
		if (!S_ISDIR(status.st_mode))
		{
			std::cout << "Creating empty directory " << subPath << "\n";
			if (mkdir(subPath.c_str()) < 0)
			{
				std::cout << "Unable to create directory" << subPath << "\n";
				return false;
			}
		}
	}
	return true;
}

// This is abhorrently non-portable and not at all worthwhile
// Can pretend to be cool though everytime you generate a seed
uint32_t genSeed()
{
	void* mem = malloc(sizeof(int));
	struct timeval time;
	gettimeofday(&time, nullptr);

	uint32_t seed = ((intptr_t)mem>>16 & 0xf)<<28;
	seed |= (time.tv_sec & 0xf)<<24;
	seed |= (getpid()>>8 & 0xf)<<20;
	seed |= (time.tv_sec>>4 & 0xf)<<16;
	seed |= (time.tv_usec & 0xfff)<<4;
	seed |= (getpid()>>4 & 0xf);

	free(mem);
	return seed;
}
