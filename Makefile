WARNINGS := -pedantic -Wall -Wextra -Walloc-zero -Wbool-compare -Wcast-align -Wcast-qual -Wchar-subscripts -Wchkp -Wdangling-else -Wdisabled-optimization -Wduplicated-branches -Wduplicated-cond -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlogical-not-parentheses -Wlogical-op -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wnoexcept-type -Wnormalized=nfc -Woverloaded-virtual -Wpointer-arith -Wregister -Wrestrict -Wsign-promo -Wsizeof-array-argument -Wstack-protector -Wstrict-aliasing=3 -Wstrict-null-sentinel -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-override -Wswitch-bool -Wundef -Wunreachable-code -Wunused -Wunused-local-typedefs -Wuseless-cast -Wvariadic-macros -Wwrite-strings -Wzero-as-null-pointer-constant -Wno-multichar -Wno-unused-parameter

OPTIMISATIONS := -O3 -frename-registers
# OPTIMISATIONS += -flto
# Link-time optimizing makes the linking super long so it's are unactivated by default

CXXFLAGS := $(CXX) $(OPTIMISATIONS) $(WARNINGS) `sdl2-config --cflags` -std=c++17 -IJson_Modern_Cpp -c -MMD -MP -MF $@.d
LDFLAGS := $(CXX) $(OPTIMISATIONS) $(WARNINGS) -s -static

MAIN := main
# game states
MAIN += game inventory mapSystem stageSelect
# modules
MAIN += filesystem flags input loadConfig log mathUtils script stage stdUtils
# collision
MAIN += bossCollision bulletCollision npcCollision playerCollision
# drawing
MAIN += fade flash hud render
# libraries
MAIN += lodepng/lodepng
# sound
MAIN +=	org pxt sound
# classes
MAIN += boss bullet caret npc player weapons valueview
# weapon acts
MAIN += bladeThrow bubblePew fireballShoot machineGunShoot missileShoot nemesisShoot polarStarShoot snakeShoot spurShoot
# bullet acts
MAIN += blade bubbler fireball missile polarStar machineGun misc nemesis snake spur
# npc acts
MAIN += npcAct npc000 npc020 npc040 npc060 npc080 npc100 npc120 npc140 npc160 npc180 npc200 npc220 npc240 npc260 npc280 npc300 npc320 npc340
# boss acts
MAIN += balfrog heavyPress monsterX omega core

OBJS := $(addprefix obj/, $(addsuffix .o, $(MAIN)))
DEPS := $(addsuffix .d, $(OBJS))

all: bin/CaveStoryEngine

bin/CaveStoryEngine: $(OBJS)
	@mkdir -p $(@D)
	$(LDFLAGS) $(OBJS) -o $@

# general compile
obj/%.o: source/%.cpp
	@mkdir -p $(@D)
	$(CXXFLAGS) $< -o $@

obj/lodepng/lodepng.o: source/lodepng/lodepng.cpp
	@mkdir -p $(@D)
	$(CXXFLAGS) $< -o $@ -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_CXXFLAGS -Wno-zero-as-null-pointer-constant -Wno-suggest-attribute=pure -Wno-suggest-attribute=const -Wno-alloc-zero -Wno-useless-cast -Wno-cast-qual
	
# Detect toolchain or OS by different methods
# First check if PATH contains any ';' (that's the seperator on Windows ; afaik nobody sane uses it on any other OS for any purpose so if it contains any of them we're assuming this is Windows)
ifeq '$(findstring ;,$(PATH))' ';'
    detected_OS := Windows
else
	# Then we get the name through uname
    detected_OS := $(shell uname 2>/dev/null || echo __UnknownOS)
	# And then detect some common Unix-on-windows toolchains
    detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
    detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
    detected_OS := $(patsubst MINGW%,MINGW,$(detected_OS))
endif

# If we're on MINGW, we can use windows icons
ifeq ($(detected_OS),MINGW)
	detected_OS := Windows
endif
	
ifeq ($(detected_OS),Windows)
	# Embed icons as resources, and load them natively
	OBJS += obj/icon.o
	CXXFLAGS += -DUSE_ICONS_WINDOWS
	LDFLAGS += `sdl2-config --static-libs`
	
obj/icon.o: res/icon.rc res/icon_mini.ico
	@mkdir -p $(@D)
	@windres $< $@
else
	# Embed a bitmap in the binary, and load it with SDL
	OBJS += obj/icon_mini.o
	LDFLAGS += `sdl2-config --libs`
	
obj/icon_mini.o: res/icon_mini.png
	@mkdir -p $(@D)
	@$(LD) -r -b binary -o $@ $<
endif
	
include $(wildcard $(DEPS))

# Remove all objects files and the binary
clean:
	@rm -rf obj bin/CaveStoryEngine
