# Yeah it's lazy... but it works for now.
files = src/config.cpp src/main.cpp src/randomizer.cpp src/table.cpp src/utils.cpp

.PHONY: all debug release clean
all: debug

debug: export cxx_flags = -Wall -g -Og

release: export cxx_flags = -O2
release: export linker_flags = -Wl,-O,--gc-sections,--strip-all

debug release: bin/StormLib.dll bin/randomizer.exe

bin/randomizer.exe: $(files)
	g++ $(cxx_flags) $(files) -lStormLib -Llib -Ilib/include -Iinclude $(linker_flags) -o $@

bin/StormLib.dll:
	cp lib/StormLib.dll bin/StormLib.dll

clean:
	rm bin/randomizer.exe
