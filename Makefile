WARNINGS := -pedantic -Wall -Wextra -Wabi -Walloc-zero -Wbool-compare -Wcast-align -Wcast-qual -Wchar-subscripts -Wchkp -Wdangling-else -Wdisabled-optimization -Wduplicated-branches -Wduplicated-cond -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winvalid-pch -Wlogical-not-parentheses -Wlogical-op -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wnoexcept-type -Wnormalized=nfc -Woverloaded-virtual -Wpointer-arith -Wregister -Wrestrict -Wsign-promo -Wsizeof-array-argument -Wstack-protector -Wstrict-aliasing=3 -Wstrict-null-sentinel -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-override -Wswitch-bool -Wundef -Wunreachable-code -Wunused -Wunused-local-typedefs -Wuseless-cast -Wvariadic-macros -Wwrite-strings -Wzero-as-null-pointer-constant -Wno-multichar -Wno-unused-parameter

OPTIMISATIONS := -O3 -frename-registers
# OPTIMISATIONS += -flto

COMPILE_CPP = $(CXX) -m32 $(OPTIMISATIONS) $(WARNINGS) -std=c++17 -I/mingw32/include/SDL2/ -IJson_Modern_Cpp -c -DUSE_ICONS_WINDOWS -MMD -MP -MF $@.d
# Replace mingw32 with usr for Unix build

LINK_CPP := $(CXX) -m32 $(OPTIMISATIONS) $(WARNINGS) -s -static `sdl2-config --static-libs`
# Remove -mwindows for Unix build

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

# Embed icons as resources, and load them natively
OBJS += obj/icon.o

all: bin/CaveStoryEngine

bin/CaveStoryEngine: $(OBJS)
	@mkdir -p $(@D)
	$(LINK_CPP) $(OBJS) -o $@
# Remove -lmingw32 for actual Unix build maybe ? Also you should probably remove the ".dll"s at the end of SDL2.dll (Maybe replace them with ".so" ?)

# general compile
obj/%.o: source/%.cpp
	@mkdir -p $(@D)
	$(COMPILE_CPP) $< -o $@

obj/lodepng/lodepng.o: source/lodepng/lodepng.cpp
	@mkdir -p $(@D)
	$(COMPILE_CPP) $< -o $@ -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_CPP -Wno-zero-as-null-pointer-constant -Wno-suggest-attribute=pure -Wno-suggest-attribute=const -Wno-alloc-zero -Wno-useless-cast -Wno-cast-qual

obj/icon.o: res/icon.rc res/icon_mini.ico
	@mkdir -p $(@D)
	@windres $< $@
# basically only works on windows though (refer to MakefileAlt)
	
include $(wildcard $(DEPS))

# Remove all objects files and the binary
clean:
	rm -rf obj bin/CaveStoryEngine
