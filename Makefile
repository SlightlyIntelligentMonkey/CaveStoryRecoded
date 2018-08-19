COMPILE_C := $(CC) -m32 -O3 -c -I/mingw32/include/SDL2/ -Wno-multichar -mwindows
COMPILE_CPP := $(CXX) -m32 -O3 -c -I/mingw32/include/SDL2/ -Wno-multichar -mwindows
# Replace mingw32 with usr for actual Unix build
LINK_CPP := $(CXX) -O3
LINK_C := $(CC) -O3

MAIN := bullet bulletCollision caret fade filesystem fireball flags game hud input level main mathUtils
MAIN += npc npcAct npcCollision org player playerCollision polarStar polarStarShoot
MAIN += render script sound spur spurShoot valueview weapons

MAIN += npc000 npc020 npc040 npc060 npc080 npc100 npc120 npc140 npc180 npc200 npc220 npc240 npc280 npc300 npc340

OBJS := $(addprefix obj/, $(addsuffix .o, $(MAIN)))

all: bin/CaveStoryRemake

bin/CaveStoryRemake: $(OBJS)
	$(LINK_CPP) -lmingw32 -lSDL2Main -lSDL2 -lSDL2_image -lm $(OBJS) -o bin/CaveStoryRemake

# general compile

obj/%.o: source/%.cpp
	$(COMPILE_CPP) $^ -o $@

# cleanup

clean:
	rm obj/*
