CXX=g++-10
GDB_FLAGS = -ggdb -g
DEBUG_FLAGS=-DSHOW_RETICLE=1 \
						-DDRAW_SPETSNAZ_PREFIRE_LINE=1 \
						-DDRAW_COLLISIONS=1 \
						-DSHOW_PATHFINDER_ANIMATE=1
SUFFIX_FLAGS=-lSDL2 -lSDL2_ttf
DEBUG_FLAGS=
#GDB_FLAGS=
BUILD_DIR=build
C_STD_FLAGS=-std=c++20 -ISDL_ttf -LSDL_ttf/build
INCLUDES_FLAGS=
SRC_DIR=src
buildobj = actor background behaviour-tree bullet \
	circle clock colors coordinates cursor debug direction draw \
	draw-state/init draw-state/player \
	draw-state/reticle font gameplay gameplay/npc-spawning \
	graphical-decay hallway line map movement \
	mp5 npc/paths npc-id npc-spetsnaz tiled/parser player \
	randomized-maps/building-generator \
	randomized-maps/fragments/square \
	rng tick timeline \
	triangle viewport wall world

default: all
objects = $(patsubst %,$(BUILD_DIR)/%.o,$(buildobj))

clean: 
	rm -f $(BUILD_DIR)/* $(BUILD_DIR)/gameplay/* $(BUILD_DIR)/draw-state/* \
		$(BUILD_DIR)/randomized-maps/fragments/* $(BUILD_DIR)/randomized-maps/* \
		$(BUILD_DIR)/tiled/* $(BUILD_DIR)/npc/*

build-dir:
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/gameplay $(BUILD_DIR)/draw-state $(BUILD_DIR)/randomized-maps/fragments $(BUILD_DIR)/tiled $(BUILD_DIR)/npc

all: $(objects)
	$(CXX) $(DEBUG_FLAGS) $(GDB_FLAGS) $(C_STD_FLAGS)  \
		$(INCLUDES_FLAGS) \
		./src/main.cpp \
		$(objects) \
		-o ./build/xorlock \
		$(SUFFIX_FLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(DEBUG_FLAGS) $(GDB_FLAGS) $(C_STD_FLAGS)  \
		-c $(FLAGS) $(INCLUDES_FLAGS) $(SUFFIX_FLAGS) -o $@ $<

-include $(objects:.o=.d)
