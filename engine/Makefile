CXX=g++-10
GDB_FLAGS=-ggdb -g -fsanitize=address -fno-omit-frame-pointer
GDB_FLAGS=-ggdb -g
DEBUG_FLAGS=-DNO_MUSIC=1 -DNO_RECOIL=1 -DNO_WEAPON_ACCURACY=1  \
						#-DTEST_DROPS=1 -DAMMO_CONSUMED=0
						#-DWEAPON_SHOULD_ALWAYS_FIRE=1 #\
						#-DTEST_TURRET=1 #-DWEAPON_SHOULD_ALWAYS_FIRE=1 #\
						-DAMMO_CONSUMED=5 #\
	-DTEST_NPC_SLASHERS=1 -DAMMO_CONSUMED=0 \
	-DTEST_SLASHER=1
#-DWEAPON_SHOULD_ALWAYS_FIRE=1 # -DDRAW_SURROUNDING_BULLET_TRAIL_RECT #\
						-DDRAW_OVERLAY_GRID=1 \
						-DDRAW_BULLET_LINE=1
#-DTEST_DROPS=1
#-DDRAW_OVERLAY_GRID=1
#-DTEST_PLAYER_EXPLOSIONS=1 -DAMMO_CONSUMED=0
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
SUFFIX_FLAGS=-lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image \
						 -ldl
#GDB_FLAGS=
#OPEN_MP=-fopenmp
OPEN_MP=
BUILD_DIR=build
C_STD_FLAGS=-std=c++20 -ISDL_ttf $(OPEN_MP) \
						-LSDL_ttf/build \
						-I$(HOME)/progs/SDL_mixer/build-prefix/include \
						-I$(HOME)/progs/SDL/build-prefix/include/SDL2 \
						-L$(HOME)/progs/SDL_mixer/builder-prefix/lib \
						-L$(HOME)/progs/SDL/builder-prefix/lib
INCLUDES_FLAGS=
SRC_DIR=src
buildobj = abilities/turret abilities/drone ability actor air-support/f35 background backpack behaviour-tree bullet \
	circle clock colors coordinates cursor db debug direction draw \
	draw-state/ammo draw-state/backpack draw-state/init draw-state/player \
	draw-state/reticle damage/explosions font gameplay gameplay/npc-spawning \
	sound/gunshot sound/reload sound/npc sound/menu \
	gameplay/waves sound/ambience \
	graphical-decay hallway \
	line loot \
	map movement \
	npc/paths npc-id npc-bomber npc-spetsnaz tiled/parser player \
	randomized-maps/building-generator \
	randomized-maps/fragments/square \
	reload rng tick time timeline \
	triangle viewport wall \
	events/death map-connections \
	weapons/assault-rifles/g36c \
	weapons/weapon-loader \
	weapons/pistol/p226 weapons/pistol weapons/primary \
	weapons/smg/mp5 weapons/grenade/frag weapons/grenade \
	window \
	world

default: all
objects = $(patsubst %,$(BUILD_DIR)/%.o,$(buildobj))

clean: 
	rm -f $(BUILD_DIR)/* $(BUILD_DIR)/gameplay/* $(BUILD_DIR)/draw-state/* \
		$(BUILD_DIR)/randomized-maps/fragments/* $(BUILD_DIR)/randomized-maps/* \
		$(BUILD_DIR)/tiled/* $(BUILD_DIR)/npc/* $(BUILD_DIR)/weapons/assault-rifles/* \
		$(BUILD_DIR)/weapons/smg/* $(BUILD_DIR)/sound/* $(BUILD_DIR)/air-support/* \
		$(BUILD_DIR)/damage/* $(BUILD_DIR)/weapons/pistol/* \
		$(BUILD_DIR)/weapons/* \
		$(BUILD_DIR)/weapons/grenade/* $(BUILD_DIR)/events/* \
		$(BUILD_DIR)/abilities/*

build-dir:
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/gameplay $(BUILD_DIR)/draw-state $(BUILD_DIR)/randomized-maps/fragments $(BUILD_DIR)/tiled $(BUILD_DIR)/npc $(BUILD_DIR)/weapons/assault-rifles/ $(BUILD_DIR)/weapons/smg/ $(BUILD_DIR)/sound $(BUILD_DIR)/air-support $(BUILD_DIR)/damage/ $(BUILD_DIR)/weapons/pistol \
		$(BUILD_DIR)/weapons/grenade \
		$(BUILD_DIR)/events/death \
		$(BUILD_DIR)/abilities/

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
