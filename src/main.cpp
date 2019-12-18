#include "config.hh"
#include "randomizer.hh"
#include "table.hh"
#include "utils.hh"
#include <StormLib.h>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

static const char* VERSION = "0.1";

// Should probably move/clean up everything to do with managing the install
// Clear up all these globals
void runRandomizer();
bool validInstall();
bool exportSetup(const std::string &file);
bool extractFiles();

std::string basePath;
std::string corePath;
std::string modPath;
std::string stagePath;
std::string dataPath;

int main()
{
	runRandomizer();

	std::cout << "(Press Enter)" << std::flush;
	std::cin.ignore(INT_MAX, '\n');
	return 0;
}

void runRandomizer()
{
	Config &cfg = Config::getInstance();
	if (!cfg.load("config.txt"))
	{
		std::cout << "Invalid config.txt. Aborting.\n" << std::flush;
		return;
	}

	if (!validInstall())
	{
		std::cout << "Invalid Diablo II installation directory. Aborting.\n" << std::flush;
		return;
	}

	if (!extractFiles())
	{
		std::cout << "Unable to extract data files. Aborting.\n" << std::flush;
		return;
	}

	uint32_t seed = 0;
	if (!cfg.getInt("seed", seed) || seed == 0)
	{
		seed = genSeed();
	}

	std::fstream sFile;
	sFile.open(modPath + "/randomizer/seed.txt", std::fstream::out | std::fstream::trunc);
	if (sFile.good())
	{
		sFile << "Files generated using seed: " << seed << "\n";
	}
	sFile.close();

	Randomizer random{stagePath, dataPath};
	if (!random.scramble(seed))
	{
		std::cout << "Unable to finish randomizer. Aborting.\n" << std::flush;
		return;
	}

	if (!random.writeFiles())
	{
		std::cout << "Unable to export modified data files. Aborting.\n" << std::flush;
		return;
	}

	std::cout << "Success! make sure to play with \"-direct -txt\".\n";
}

bool validInstall()
{
	Config &cfg = Config::getInstance();
	// basePath = d2path in config.txt
	if (!cfg.getString("d2path", basePath))
	{
		return false;
	}
	// modPath appends D2SE mod directory if d2se is enabled in config.txt
	modPath = basePath;
	// corePath appends D2SE core directory if d2se is enabled in config.txt
	corePath = basePath;

	bool success = true;
	if (cfg.getBool("d2se"))
	{
		// If D2SE install path, check for D2SE.ini
		std::fstream peekFile;
		peekFile.open(basePath + "/D2SE.ini", std::fstream::in);
		if (!peekFile.good())
		{
			printError(EC::badInstall, "D2SE");
			success = false;
		}
		peekFile.close();

		success &= cfg.getString("d2se_core", corePath);
		corePath = basePath + "/D2SE/CORES/" + corePath;
		success &= cfg.getString("d2se_mod", modPath);
		modPath = basePath + "/MODS/" + modPath;

		// Only attempt to create D2SE_SETUP.ini if we believe d2path to still be valid
		if (success && mkdir_r(modPath))
		{
			// Skip export if already exists
			peekFile.open(modPath + "/D2SE_SETUP.ini", std::fstream::in);
			if (!peekFile.good())
			{
				success = exportSetup(modPath + "/D2SE_SETUP.ini");
			}
			peekFile.close();
		}
		else
		{
			success = false;
		}
	}
	else
	{
		// If vanilla D2 install path, check for patch_d2.mpq
		std::fstream peekFile;
		peekFile.open(basePath + "/patch_d2.mpq", std::fstream::in);
		if (!peekFile.good())
		{
			printError(EC::badInstall, "Diablo II");
			success = false;
		}
		peekFile.close();
	}

	std::cout << "Using installation directory \"" << basePath << "\".\n";
	stagePath = modPath + "/randomizer/vanillafiles";
	dataPath = modPath + "/data/global/excel";
	// Only attempt to create subdirs if we believe d2path to still be valid
	if (success && !(mkdir_r(stagePath) && mkdir_r(dataPath)))
	{
		success = false;
	}

	return success;
}

bool exportSetup(const std::string &file)
{
	Config &cfg = Config::getInstance();
	bool success = true;
	std::string coreName;
	std::string modName;
	success = cfg.getString("d2se_core", coreName) &&
		cfg.getString("d2se_mod", modName);

	// Only write to file if config options are present
	if (success)
	{
		std::cout << "Exporting default D2SE_SETUP.ini\n";
		std::fstream setup;
		setup.open(file, std::fstream::out | std::fstream::trunc);
		if(setup.good())
		{
			setup << "[D2SE]\n"
				"CreatedFor=2.2.0\n\n"
				"[USERSETTINGS]\n"
				"Language=MOD\n"
				"Direct=1\n"
				"Txt=1\n\n"
				"[Protected]\n"
				"D2Core=" << coreName << "\n\n"
				"ModName=" << modName << "\n"
				"ModTitle=" << modName << "\n"
				"ModMajorVersion=" << VERSION << "\n\n"
				"ModAllowPlugY=0\n"
				"Modable=1\n"
				"ModReadme=randomizer\\seed.txt\n"
				"[END-Protected]\n\n"
				"#dont touch this it must be at the end of file to check for corruptions\n"
				"[END]\n"
				"endoffile=4711\n";
		}
		else
		{
			printError(EC::badAccess, file);
			success = false;
		}
		setup.close();
	}

	return success;
}

bool extractFiles()
{
	std::cout << "Opening mpq archives.\n";

	HANDLE hMpq;
	std::string filePath = basePath + "/" + "d2data.mpq";
	if (!SFileOpenArchive(filePath.c_str(), 0, STREAM_FLAG_READ_ONLY, &hMpq))
	{
		printError(EC::badAccess, filePath);
		return false;
	}
	filePath = basePath + "/" + "d2exp.mpq";
	if (!SFileOpenPatchArchive(hMpq, filePath.c_str(), nullptr, 0))
	{
		printError(EC::badAccess, filePath);
		return false;
	}
	filePath = corePath + "/" + "patch_d2.mpq";
	if (!SFileOpenPatchArchive(hMpq, filePath.c_str(), nullptr, 0))
	{
		printError(EC::badAccess, filePath);
		return false;
	}

	std::cout << "Exctacting data files.\n";
	for (size_t i = 0; i < File::TOTAL_FILES; i++)
	{
		HANDLE hFile;
		filePath = "data\\global\\excel\\" + FILE_NAME[i];
		if (!SFileOpenFileEx(hMpq, filePath.c_str(), SFILE_OPEN_FROM_MPQ, &hFile))
		{
			printError(EC::badMpqAccess, filePath);
			SFileCloseArchive(hMpq);
			return false;
		}

		DWORD fileSize = SFileGetFileSize(hFile, nullptr);
		DWORD bRead = 0;
		char* buffer = new char[fileSize];
		SFileReadFile(hFile, buffer, fileSize, &bRead, nullptr);
		SFileCloseFile(hFile);

		std::fstream dest;
		filePath = stagePath + "/" + FILE_NAME[i];
		dest.open(filePath, std::fstream::out | std::fstream::trunc | std::fstream::binary);
		if (!dest.good())
		{
			printError(EC::badAccess, filePath);
			SFileCloseArchive(hMpq);
			return false;
		}

		dest.write(buffer, bRead);

		delete[] buffer;
		dest.close();
	}

	SFileCloseArchive(hMpq);
	return true;
}
