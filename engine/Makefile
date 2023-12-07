CXX=g++-10
GDB_FLAGS=-ggdb -g -fsanitize=address -fno-omit-frame-pointer
GDB_FLAGS=-ggdb -g
DEBUG_FLAGS=-DNO_MUSIC=1 -DTEST_PLAYER_EXPLOSIONS=1 -DAMMO_CONSUMED=0
#-DEXPLOSION_DAMAGE=1 -DAMMO_CONSUMED=0
#-DF35_STRESS_TEST=1
#-DEXPLOSION_DIVISOR=5
RELEASE_FLAGS=
XORLOCK_BIN=./build/xorlock
#-DDRAW_BULLET_LINE=1
#-DSHOW_RETICLE=1 \
#-DDRAW_OVERLAY_GRID=1 \
						#-DDRAW_SPETSNAZ_PREFIRE_LINE=1 \
						#-DDRAW_COLLISIONS=1
						#-DSHOW_HELPFUL_GRAPH_STUFF=1
						#-DSHOW_PATHFINDER_ANIMATE=1
SUFFIX_FLAGS=-lSDL2 -lSDL2_ttf -lSDL2_mixer
#GDB_FLAGS=
BUILD_DIR=build
C_STD_FLAGS=-std=c++20 -ISDL_ttf -LSDL_ttf/build -I$(HOME)/progs/SDL_mixer/build-prefix/include -I$(HOME)/progs/SDL/build-prefix/include/SDL2 -L$(HOME)/progs/SDL_mixer/builder-prefix/lib -L$(HOME)/progs/SDL/builder-prefix/lib
INCLUDES_FLAGS=
SRC_DIR=src
buildobj = actor air-support/f35 background behaviour-tree bullet \
	circle clock colors coordinates cursor db debug direction draw \
	draw-state/ammo draw-state/init draw-state/player \
	draw-state/reticle damage/explosions font gameplay gameplay/npc-spawning \
	sound/gunshot sound/reload sound/npc sound/menu \
	gameplay/waves sound/ambience \
	graphical-decay hallway line map movement \
	npc/paths npc-id npc-spetsnaz tiled/parser player \
	randomized-maps/building-generator \
	randomized-maps/fragments/square \
	reload rng tick time timeline \
	triangle viewport wall \
	weapons/assault-rifles/g36c \
	weapons/weapon-loader \
	weapons/pistol/p226 \
	weapons/smg/mp5 \
	world

default: all
objects = $(patsubst %,$(BUILD_DIR)/%.o,$(buildobj))

clean: 
	rm -f $(BUILD_DIR)/* $(BUILD_DIR)/gameplay/* $(BUILD_DIR)/draw-state/* \
		$(BUILD_DIR)/randomized-maps/fragments/* $(BUILD_DIR)/randomized-maps/* \
		$(BUILD_DIR)/tiled/* $(BUILD_DIR)/npc/* $(BUILD_DIR)/weapons/assault-rifles/* \
		$(BUILD_DIR)/weapons/smg/* $(BUILD_DIR)/sound/* $(BUILD_DIR)/air-support/* \
		$(BUILD_DIR)/damage/* $(BUILD_DIR)/weapons/pistol/*

build-dir:
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/gameplay $(BUILD_DIR)/draw-state $(BUILD_DIR)/randomized-maps/fragments $(BUILD_DIR)/tiled $(BUILD_DIR)/npc $(BUILD_DIR)/weapons/assault-rifles/ $(BUILD_DIR)/weapons/smg/ $(BUILD_DIR)/sound $(BUILD_DIR)/air-support $(BUILD_DIR)/damage/ $(BUILD_DIR)/weapons/pistol

wavelengths:
	cd $(SRC_DIR)/scripts && ./wavlen.sh

all: $(objects)
	$(CXX) $(DEBUG_FLAGS) $(GDB_FLAGS) $(C_STD_FLAGS)  \
		$(INCLUDES_FLAGS) \
		./src/main.cpp \
		$(objects) \
		-o $(XORLOCK_BIN) \
		$(SUFFIX_FLAGS)

cachegrind: all
	valgrind --tool=callgrind $(XORLOCK_BIN); exit 0

annotate:
	callgrind_annotate ./callgrind.out.*

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(DEBUG_FLAGS) $(GDB_FLAGS) $(C_STD_FLAGS)  \
		-c $(FLAGS) $(INCLUDES_FLAGS) $(SUFFIX_FLAGS) -o $@ $<

-include $(objects:.o=.d)
