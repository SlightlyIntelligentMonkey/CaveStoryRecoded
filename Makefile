# Detect toolchain or OS by different methods
# First check if PATH contains any ';' (that's the seperator on Windows ; afaik nobody sane uses it on any other OS for any purpose so if it contains any of them we're assuming this is Windows)
ifeq '$(findstring ;,$(PATH))' ';'
    detected_OS := Windows
else
	# Then we get the name through uname
    detected_OS := $(shell uname 2>/dev/null || echo __UnknownOS)
	# And then detect some common Unix-on-Windows toolchains
    detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
    detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
    detected_OS := $(patsubst MINGW%,MINGW,$(detected_OS))
endif

# If we're on MINGW, we can use native Windows stuff
ifeq ($(detected_OS),MINGW)
	detected_OS := Windows
endif

ifeq ($(RELEASE), 1)
TYPEOFBUILD := Release
else
TYPEOFBUILD := Debug
endif

# Warning options fed to the compiler
WARNINGS := -pedantic -Wall -Wextra -Walloc-zero -Walloca -Wbool-compare -Wcast-align -Wcast-qual -Wchar-subscripts -Wchkp -Wctor-dtor-privacy -Wdangling-else -Wdisabled-optimization -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond -Wfloat-conversion -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlogical-not-parentheses -Wlogical-op -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wnoexcept-type -Wnon-virtual-dtor -Wnormalized=nfc -Wold-style-cast -Woverloaded-virtual -Wpointer-arith -Wregister -Wreorder -Wrestrict -Wshadow -Wsizeof-array-argument -Wstack-protector -Wstrict-aliasing=3 -Wstrict-null-sentinel -Wsuggest-attribute=const -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-override -Wstrict-null-sentinel -Wswitch-bool -Wundef -Wunreachable-code -Wunused -Wunused-local-typedefs -Wunused-macros -Wunused-parameter -Wvariadic-macros -Wwrite-strings -Wzero-as-null-pointer-constant -Wno-multichar -Wno-unused-parameter

# Optimisation options fed to the compiler
ifeq ($(RELEASE), 1)
OPTIMISATIONS := -O3 -frename-registers -flto -s
else
OPTIMISATIONS := -Og -ggdb
endif

# Base command-line when calling the compiler
# `sdl2-config --cflags` to get the flags needed to compile with SDL2
# -std=c++17 to activate C++17 features
# -MMD -MP -MF $@.d to make the compiler generate dependency files
# -c so the compiler makes a simple compile into an object file
# -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_CPP so that lodepng doesn't compile those
CXXFLAGS = $(CXX) $(OPTIMISATIONS) $(WARNINGS) `sdl2-config --cflags` -std=c++17 -c -MMD -MP -MF $@.d -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_CPP
LDFLAGS := $(CXX) $(OPTIMISATIONS) $(WARNINGS)
ifeq ($(RELEASE), 1)
LDFLAGS += -s
endif

ifeq ($(detected_OS),Windows)
# We can use native icons
	CXXFLAGS += -DUSE_ICONS_WINDOWS
endif

ifeq ($(STATIC), 1)
	LDFLAGS += -static `sdl2-config --static-libs`
else
	LDFLAGS += `sdl2-config --libs`
endif

# Main source file
MAIN := main
# States the game can be in
MAIN += game inventory mapSystem stageSelect
# Random utilities
MAIN += filesystem flags input loadConfig log mathUtils script shiftJISToUTF8 stage stdUtils
# Collision handling
MAIN += bossCollision bulletCollision npcCollision playerCollision
# Drawing-related
MAIN += fade flash hud render
# External Libraries
MAIN += lodepng/lodepng
# Sound-related
MAIN +=	org pxt sound
# Classes
MAIN += boss bullet caret npc player weapons valueview
# Weapon behavior
MAIN += bladeThrow bubblePew fireballShoot machineGunShoot missileShoot nemesisShoot polarStarShoot snakeShoot spurShoot
# Bullet behavior
MAIN += blade bubbler fireball missile polarStar machineGun misc nemesis snake spur
# NPC behavior
MAIN += npcAct npc000 npc020 npc040 npc060 npc080 npc100 npc120 npc140 npc160 npc180 npc200 npc220 npc240 npc260 npc280 npc300 npc320 npc340 npc360
# Boss behavior
MAIN += balfrog ballos heavyPress monsterX omega core

OBJS := $(addprefix obj/, $(addprefix $(TYPEOFBUILD)/, $(addsuffix .o, $(MAIN))))
ifeq ($(detected_OS),Windows)
	# Embed icons as resources, and load them natively
	OBJS += obj/$(TYPEOFBUILD)/icon.o
endif
DEPS := $(addsuffix .d, $(OBJS))

# Default target
all: bin/$(TYPEOFBUILD)/CaveStoryRecoded

# CaveStoryRecoded binary
bin/$(TYPEOFBUILD)/CaveStoryRecoded: $(OBJS)
	@mkdir -p $(@D)
	@echo Linking to $@...
	@$(LDFLAGS) $(OBJS) -o $@
	@echo Finished compiling $@
	@echo Copying data files...
	@rsync -a gameData/ bin/$(TYPEOFBUILD)/
	@echo Copied data files

# Generic source compile
obj/$(TYPEOFBUILD)/%.o: source/%.cpp
	@mkdir -p $(@D)
	@echo Compiling $<...
	@$(CXXFLAGS) $< -o $@
	@echo Finished compiling $<

# Compile for lodepng (avoid warnings and also some other stuff)
obj/$(TYPEOFBUILD)/lodepng/lodepng.o: source/lodepng/lodepng.cpp
	@mkdir -p $(@D)
	@echo Compiling $<...
	@$(CXXFLAGS) $< -o $@ -Wno-zero-as-null-pointer-constant -Wno-alloc-zero -Wno-cast-qual -Wno-old-style-cast -Wno-unused-macros -Wno-suggest-attribute=const
	@echo Finished compiling $<
	
ifeq ($(detected_OS),Windows)
# Compile icon resource
obj/$(TYPEOFBUILD)/icon.o: res/icon.rc res/icon_mini.ico
	@mkdir -p $(@D)
	@windres $< $@
else
# Compile icon from file
source/icon_mini.h: res/icon_mini.png
	@xxd -i $< $@
endif

# Include dependencies
include $(wildcard $(DEPS))

# Remove all objects files and the binary
clean:
	@rm -rf obj bin
