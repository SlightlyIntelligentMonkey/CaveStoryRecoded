COMPILE_C := $(CC) -O3 -c -I/usr/include/SDL2
COMPILE_CPP := $(CXX) -O3 -c -I/usr/include/SDL2
LINK_CPP := $(CXX) -O3
LINK_C := $(CC) -O3

MAIN := filesystem flags input level main math
MAIN += npc npcAct npcCollision player playerCollision
MAIN += render script sound

MAIN += npc000 npc020 npc040 npc060 npc200 npc340

OBJS := $(addprefix obj/, $(addsuffix .o, $(MAIN)))

# liborganya
OBJS += obj/liborganya-file.o obj/liborganya-decoder.o

# Compile liborganya's org2dat for convenience while testing modifications.
all: bin/CaveStoryRemake bin/org2dat

bin/CaveStoryRemake: $(OBJS)
	$(LINK_CPP) -lSDL2 -lm $(OBJS) -o bin/CaveStoryRemake

bin/org2dat: obj/liborganya-file.o obj/liborganya-decoder.o obj/liborganya-org2dat.o
	$(LINK_C) -lm obj/liborganya-file.o obj/liborganya-decoder.o obj/liborganya-org2dat.o -o bin/org2dat

# liborganya stuff.
# Due to the modifications, this must have access to the SDL2 endianness includes.
obj/liborganya-file.o: liborganya/file.c
	$(COMPILE_C) liborganya/file.c -o obj/liborganya-file.o

obj/liborganya-decoder.o: liborganya/decoder.c
	$(COMPILE_C) liborganya/decoder.c -o obj/liborganya-decoder.o

obj/liborganya-org2dat.o: liborganya/org2dat.c
	$(COMPILE_C) liborganya/org2dat.c -o obj/liborganya-org2dat.o

# general compile

obj/%.o: source/%.cpp
	$(COMPILE_CPP) $^ -o $@

# cleanup

clean:
	rm obj/*
