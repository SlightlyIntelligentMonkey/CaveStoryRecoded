WARNINGS := -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wundef -Werror -Wunused -Wno-multichar -Wno-unused-parameter
COMPILE_C := $(CC) -m32 -O3 -flto -c $(WARNINGS) -std=c++1z -I/mingw32/include/SDL2/ 
COMPILE_CPP := $(CXX) -m32 -O3 -flto -c $(WARNINGS) -std=c++1z -I/mingw32/include/SDL2/ 
# Replace mingw32 with usr for actual Unix build
LINK_CPP := $(CXX) -m32 -O3 -flto -static -static-libstdc++ -static-libgcc -s
LINK_C := $(CC) -m32 -O3 -flto -static -static-libstdc++ -static-libgcc -s

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
