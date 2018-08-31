WARNINGS := -pedantic -Wall -Wextra -Wabi -Walloc-zero -Wbool-compare -Wcast-align -Wcast-qual -Wchar-subscripts -Wchkp -Wdangling-else -Wdisabled-optimization -Wduplicated-branches -Wduplicated-cond -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport -Winit-self -Winline -Winvalid-pch -Wlogical-not-parentheses -Wlogical-op -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wnoexcept -Wnoexcept-type -Wnormalized=nfc -Woverloaded-virtual -Wpointer-arith -Wregister -Wrestrict -Wsign-promo -Wsizeof-array-argument -Wstack-protector -Wstrict-aliasing=3 -Wstrict-null-sentinel -Wsuggest-attribute=const -Wsuggest-attribute=format -Wsuggest-attribute=noreturn -Wsuggest-attribute=pure -Wsuggest-override -Wswitch-bool -Wundef -Wunreachable-code -Wunused -Wunused-local-typedefs -Wuseless-cast -Wvariadic-macros -Wwrite-strings -Wzero-as-null-pointer-constant -Wno-multichar -Wno-unused-parameter

OPTIMISATIONS := -Ofast -flto -frename-registers -funroll-loops

COMPILE_C := $(CC) -m32 $(OPTIMISATIONS) $(WARNINGS) -std=c11 -I/mingw32/include/SDL2/ -IJson_Modern_Cpp -c
COMPILE_CPP := $(CXX) -m32 $(OPTIMISATIONS) $(WARNINGS) -std=c++17 -I/mingw32/include/SDL2/ -IJson_Modern_Cpp -c
# Replace mingw32 with usr for actual Unix build
LINK_CPP := $(CXX) -m32 $(OPTIMISATIONS) $(WARNINGS) -static -static-libstdc++ -static-libgcc -mwindows
LINK_C := $(CC) -m32 $(OPTIMISATIONS) $(WARNINGS) -static -static-libstdc++ -static-libgcc -mwindows
# Remove -mwindows for Unix build

MAIN := bullet bulletCollision caret fade filesystem fireball fireballShoot flags game hud input inventory level loadConfig main mathUtils
MAIN += npc npcAct npcCollision org player playerCollision polarStar polarStarShoot
MAIN += render script sound spur spurShoot stageSelect stdUtils valueview weapons

MAIN += npc000 npc020 npc040 npc060 npc080 npc100 npc120 npc140 npc180 npc200 npc220 npc240 npc280 npc300 npc340

OBJS := $(addprefix obj/, $(addsuffix .o, $(MAIN)))

all: bin/CaveStoryRemake

bin/CaveStoryRemake: $(OBJS)
	$(LINK_CPP) $(OBJS) -lmingw32 -lSDL2Main -lSDL2.dll -lSDL2_image.dll -o $@
# Remove -lmingw32 for actual Unix build maybe ? Also prolly remove the ".dll"s at the end of SDL2.dll and SDL2_image.dll

# general compile

obj/%.o: source/%.cpp
	$(COMPILE_CPP) $^ -o $@

# cleanup

clean:
	rm obj/*
