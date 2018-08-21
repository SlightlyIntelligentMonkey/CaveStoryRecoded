WARNINGS := -pedantic -Wall -Wextra -Wabi -Waggregate-return -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Weffc++ -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wlogical-op -Wlong-long -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wnormalized=nfc -Wold-style-cast -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-protector -Wstrict-aliasing=1 -Wstrict-null-sentinel -Wswitch-enum -Wundef -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wuseless-cast -Wvariadic-macros -Wwrite-strings -Wzero-as-null-pointer-constant -Wno-multichar -Wno-unused-parameter

OPTIMISATIONS := -Ofast -flto -frename-registers -funroll-loops

COMPILE_C := $(CC) -m32 $(OPTIMISATIONS) $(WARNINGS) -std=c++1z -I/mingw32/include/SDL2/ -mwindows -c
COMPILE_CPP := $(CXX) -m32 $(OPTIMISATIONS) $(WARNINGS) -std=c++1z -I/mingw32/include/SDL2/ -mwindows -c
# Replace mingw32 with usr and remove -mwindows for actual Unix build
LINK_CPP := $(CXX) -m32 $(OPTIMISATIONS) -static -static-libstdc++ -static-libgcc -s
LINK_C := $(CC) -m32 $(OPTIMISATIONS) -static -static-libstdc++ -static-libgcc -s

MAIN := bullet bulletCollision caret fade filesystem fireball fireballShoot flags game hud input level main mathUtils
MAIN += npc npcAct npcCollision org player playerCollision polarStar polarStarShoot
MAIN += render script sound spur spurShoot valueview weapons

MAIN += npc000 npc020 npc040 npc060 npc080 npc100 npc120 npc140 npc180 npc200 npc220 npc240 npc280 npc300 npc340

OBJS := $(addprefix obj/, $(addsuffix .o, $(MAIN)))

all: bin/CaveStoryRemake

bin/CaveStoryRemake: $(OBJS)
	$(LINK_CPP)  $(OBJS) -lmingw32 -lSDL2Main -lSDL2.dll -lSDL2_image.dll -o bin/CaveStoryRemake
# Remove mingw32 for actual Unix build maybe ? Also prolly remove the ".dll"s at the end of SDL2.dll and SDL2_image.dll

# general compile

obj/%.o: source/%.cpp
	$(COMPILE_CPP) $^ -o $@

# cleanup

clean:
	rm obj/*
