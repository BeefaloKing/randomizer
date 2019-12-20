# Yeah it's lazy... but it works for now.
files = src/config.cpp src/main.cpp src/randomizer.cpp src/table.cpp src/utils.cpp

ifeq ($(MSYSTEM),MINGW64)
lib_gcc = libgcc_s_seh-1.dll
else ifeq ($(MSYSTEM),MINGW32)
lib_gcc = libgcc_s_dw2-1.dll
else
$(error Please build with MSYS2)
endif

lib_dir = lib/bin/$(MSYSTEM_CARCH)
linker_flags = -lStormLib -L$(lib_dir)
exe_dir = bin/$(MSYSTEM_CARCH)
include = -Ilib/include -Iinclude

.PHONY: all debug release clean build runtime
all: debug

debug: export cxx_flags = -Wall -g -O0

release: export cxx_flags = -O2
release: export linker_options = -Wl,-O,--gc-sections,--strip-all

debug release:
	mkdir -p $(exe_dir)/licenses
	$(MAKE) build

build: $(exe_dir)/randomizer.exe $(exe_dir)/StormLib.dll $(exe_dir)/libstdc++-6.dll $(exe_dir)/readme.txt

$(exe_dir)/randomizer.exe: $(files)
	g++ $(cxx_flags) $(files) $(linker_flags) $(include) $(linker_options) -o $@

$(exe_dir)/StormLib.dll:
	cp $(lib_dir)/StormLib.dll $@
	cp -a lib/licenses/StormLib $(exe_dir)/licenses

$(exe_dir)/libstdc++-6.dll:
# Copies gcc runtime binaries
	cp $(MSYSTEM_PREFIX)/bin/$(lib_gcc) $(exe_dir)
	cp $(MSYSTEM_PREFIX)/bin/libstdc++-6.dll $(exe_dir)
	cp $(MSYSTEM_PREFIX)/bin/libwinpthread-1.dll $(exe_dir)

# Copies gcc runtime licenses
	cp -a $(MSYSTEM_PREFIX)/share/licenses/gcc-libs $(exe_dir)/licenses
	cp -a $(MSYSTEM_PREFIX)/share/licenses/libwinpthread $(exe_dir)/licenses

$(exe_dir)/readme.txt:
	cp -r files/* $(exe_dir)

clean:
	rm $(exe_dir)/randomizer.exe 2> /dev/null
